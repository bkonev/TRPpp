#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$ROOT_DIR/bin/trp++"
BASELINE="$ROOT_DIR/tests/snapshot.jsonl"

if [[ ! -x "$BIN" ]]; then
  echo "ERROR: binary not found or not executable at $BIN" >&2
  exit 1
fi
if [[ ! -f "$BASELINE" ]]; then
  echo "ERROR: baseline snapshot not found at $BASELINE" >&2
  exit 1
fi

STAMP=$(date +%Y%m%d-%H%M%S)
NEW_SNAPSHOT="$ROOT_DIR/tests/snapshot-$STAMP.jsonl"
export ROOT_DIR
export NEW_SNAPSHOT

python3 - <<'PY'
import json, subprocess, pathlib, time, re, os, sys
root = pathlib.Path(os.environ['ROOT_DIR'])
base = root / 'tests' / 'snapshot.jsonl'
bin_path = root / 'bin' / 'trp++'

snap = {}
for line in base.read_text(encoding='utf-8').splitlines():
    rec = json.loads(line)
    snap[rec['test']] = rec

new_snapshot = pathlib.Path(os.environ['NEW_SNAPSHOT'])

results = {}
for test_rel, rec in sorted(snap.items()):
    t = root / test_rel
    timeout = rec.get('timeout_sec') or 30
    try:
        proc = subprocess.run([str(bin_path), str(t)],
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              text=True,
                              timeout=timeout)
        cur = {
            'test': test_rel,
            'exit_code': proc.returncode,
            'stdout': proc.stdout,
            'stderr': proc.stderr,
            'timeout_sec': None,
        }
    except subprocess.TimeoutExpired as e:
        cur = {
            'test': test_rel,
            'exit_code': None,
            'stdout': e.stdout or '',
            'stderr': e.stderr or '',
            'timeout_sec': timeout,
        }
    results[test_rel] = cur

with new_snapshot.open('w', encoding='utf-8') as f:
    for k in sorted(results.keys()):
        f.write(json.dumps(results[k], ensure_ascii=False))
        f.write('\n')

# Timing comparison
re_time = re.compile(r'^Elapsed time\s+([0-9]*\.?[0-9]+)s', re.M)

def elapsed(rec):
    m = re_time.search(rec.get('stdout',''))
    return float(m.group(1)) if m else None

paired = []
for k, old in snap.items():
    new = results.get(k)
    if not new:
        continue
    if old.get('exit_code') == 0 and new.get('exit_code') == 0:
        t_old = elapsed(old)
        t_new = elapsed(new)
        if t_old is not None and t_new is not None:
            paired.append((k, t_old, t_new))

old_total = sum(o for _, o, _ in paired) if paired else 0.0
new_total = sum(n for _, _, n in paired) if paired else 0.0
old_avg = (old_total/len(paired)) if paired else 0.0
new_avg = (new_total/len(paired)) if paired else 0.0

# Mismatches excluding elapsed time

def normalize(s: str):
    return '\n'.join([ln for ln in s.splitlines() if not ln.startswith('Elapsed time ')])

mismatches = []
for k, old in snap.items():
    new = results.get(k)
    if not new:
        mismatches.append((k, ['missing_in_new']))
        continue
    diffs = []
    if old.get('exit_code') != new.get('exit_code'):
        diffs.append('exit_code')
    if old.get('timeout_sec') != new.get('timeout_sec'):
        diffs.append('timeout_sec')
    if normalize(old.get('stdout','')) != normalize(new.get('stdout','')):
        diffs.append('stdout')
    if old.get('stderr','') != new.get('stderr',''):
        diffs.append('stderr')
    if diffs:
        mismatches.append((k, diffs))

print('NEW_SNAPSHOT', new_snapshot)
print('Compared tests (elapsed time):', len(paired))
print('Baseline total elapsed:', old_total)
print('Current  total elapsed:', new_total)
print('Baseline avg elapsed:', old_avg)
print('Current  avg elapsed:', new_avg)
print('Faster overall:', 'current' if new_total < old_total else 'baseline')
print('MISMATCHES_EXCL_TIME', len(mismatches))
for test_rel, diffs in mismatches:
    print(test_rel, ','.join(diffs))
PY
