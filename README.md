# TRP++

Temporal resolution prover implemented in C++.

## Build Prerequisites

Required tools:
- `cmake` (3.16+)
- `ninja`
- `bison`
- `flex`
- `gengetopt`
- Boost (>= 1.83, headers required; `boost::pool` used). CMake will try config
  packages first and fall back to the built-in FindBoost module.
- C/C++ compiler (AppleClang, GCC, or Clang)

## Build (CMake + Ninja)

Configure once:
```sh
cmake -S . -B build -G Ninja
```

Build Release:
```sh
cmake --build build --target release
```

Build Debug:
```sh
cmake --build build --target debug
```

Build Profile:
```sh
cmake --build build --target profile
```

The binary is written to:
```
bin
```

## Notes

- Program name is read from `PROGNAME` and version from `VERSION`.
- The build generates parser and scanner sources from:
  - `src/builder/trp/parse.y` (bison)
  - `src/builder/trp/scan.fl` (flex)
- Command-line options are generated from:
  - `src/getopt/genoptions.ggo` (gengetopt)

## Optional Configuration

If you need an extra include directory (legacy build used `/users/loco/konev/include`), configure with:
```sh
cmake -S . -B build -G Ninja -DTRP_EXTRA_INCLUDE_DIR=/path/to/include
```

On macOS, you can optionally point CMake at a specific SDK:
```sh
cmake -S . -B build -G Ninja -DTRP_OSX_SYSROOT=/path/to/MacOSX.sdk
```

If you leave `TRP_OSX_SYSROOT` empty on macOS, CMake will auto-detect the SDK
via `xcrun` when available.
