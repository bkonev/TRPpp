# Test Comparison

This folder contains a baseline snapshot and a script to re-run tests and compare results.

## Baseline Snapshot

The baseline snapshot is stored at:

- `tests/snapshot.jsonl`

It contains one JSON record per `.in` test with stdout, stderr, exit code, and timeout info.

## Run Comparison

From the repository root:

```sh
./tests/compare.sh
```

This will:

- Re-run all tests with the same per-test timeouts used in the baseline
- Write a new snapshot to `tests/snapshot-YYYYMMDD-HHMMSS.jsonl`
- Compare running times (Elapsed time lines)
- Report mismatches excluding the `Elapsed time` line

## Notes

- If the binary is missing, build it first:

```sh
cmake --build build --target release
```
