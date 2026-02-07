# TRP++

Temporal resolution prover implemented in C++.

## Build Prerequisites

Required tools:
- `cmake` (3.16+)
- `ninja`
- `bison`
- `flex`
- `gengetopt`
- C/C++ compiler (AppleClang, GCC, or Clang)

## Build (CMake + Ninja)

Configure once:
```sh
cmake -S /Users/konev/Documents/TRP++ -B /Users/konev/Documents/TRP++/build -G Ninja
```

Build Release:
```sh
cmake --build /Users/konev/Documents/TRP++/build --target release
```

Build Debug:
```sh
cmake --build /Users/konev/Documents/TRP++/build --target debug
```

Build Profile:
```sh
cmake --build /Users/konev/Documents/TRP++/build --target profile
```

The binary is written to:
```
/Users/konev/Documents/TRP++/bin
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
cmake -S /Users/konev/Documents/TRP++ -B /Users/konev/Documents/TRP++/build -G Ninja -DTRP_EXTRA_INCLUDE_DIR=/path/to/include
```
