cmake_minimum_required(VERSION 3.16)

project(TRPXX LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure an SDK is selected on macOS (helps locate standard library headers).
if(APPLE AND NOT CMAKE_OSX_SYSROOT)
  if(EXISTS "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
    set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk" CACHE PATH "" FORCE)
  endif()
endif()

# Read program name and version from repo files.
file(READ "${CMAKE_SOURCE_DIR}/PROGNAME" _PROGNAME_CONTENT)
string(REGEX MATCH "prog=([^\n\r]+)" _PROGNAME_MATCH "${_PROGNAME_CONTENT}")
set(TRP_PROGNAME "${CMAKE_MATCH_1}")
if(NOT TRP_PROGNAME)
  set(TRP_PROGNAME "trp++")
endif()

# Convenience top-level targets to mimic legacy "make <flavor>" usage.
set(TRP_BUILD_ROOT "${CMAKE_SOURCE_DIR}/build")

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${TRP_BUILD_ROOT}/release -G Ninja -DCMAKE_BUILD_TYPE=Release
  COMMAND ${CMAKE_COMMAND} --build ${TRP_BUILD_ROOT}/release
  COMMENT "Configure and build Release with Ninja"
)

add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${TRP_BUILD_ROOT}/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
  COMMAND ${CMAKE_COMMAND} --build ${TRP_BUILD_ROOT}/debug
  COMMENT "Configure and build Debug with Ninja"
)

add_custom_target(profile
  COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${TRP_BUILD_ROOT}/profile -G Ninja -DCMAKE_BUILD_TYPE=Profile
  COMMAND ${CMAKE_COMMAND} --build ${TRP_BUILD_ROOT}/profile
  COMMENT "Configure and build Profile with Ninja"
)

file(READ "${CMAKE_SOURCE_DIR}/VERSION" _VERSION_CONTENT)
string(REGEX MATCH "VERSION=([^\n\r]+)" _VERSION_MATCH "${_VERSION_CONTENT}")
set(TRP_VERSION "${CMAKE_MATCH_1}")
if(NOT TRP_VERSION)
  set(TRP_VERSION "v0.0.0")
endif()

# Default build type for single-config generators (e.g., Ninja)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build flags aligned with the legacy Makefile.inc
set(CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_RELEASE "-O3 -fomit-frame-pointer -funroll-loops -ffast-math -mtune=generic" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -funroll-loops -ffast-math -mtune=generic" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_PROFILE "-pg -g -O3" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_PROFILE "-pg -g -O3" CACHE STRING "" FORCE)

# Dependencies for generated sources.
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_program(GENGETOPT gengetopt REQUIRED)
find_package(Boost 1.85 CONFIG REQUIRED)

set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PARSER_DIR "${GEN_DIR}/builder/trp")
set(GETOPT_DIR "${GEN_DIR}/getopt")
file(MAKE_DIRECTORY "${PARSER_DIR}")
file(MAKE_DIRECTORY "${GETOPT_DIR}")

BISON_TARGET(trp_parser
  "${CMAKE_SOURCE_DIR}/src/builder/trp/parse.y"
  "${PARSER_DIR}/parse.cc"
  DEFINES_FILE "${PARSER_DIR}/parse.h"
  COMPILE_FLAGS "-p trp_yy"
)

FLEX_TARGET(trp_scanner
  "${CMAKE_SOURCE_DIR}/src/builder/trp/scan.fl"
  "${PARSER_DIR}/scan.cc"
  COMPILE_FLAGS "-Ptrp_yy"
)

add_flex_bison_dependency(trp_scanner trp_parser)

# Select appropriate gengetopt input (repo uses flavor-suffixed files).
set(GGO_FILE "${CMAKE_SOURCE_DIR}/src/getopt/genoptions.ggo")
if(NOT EXISTS "${GGO_FILE}")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GGO_FILE "${CMAKE_SOURCE_DIR}/src/getopt/genoptions.ggo_debug")
  elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
    set(GGO_FILE "${CMAKE_SOURCE_DIR}/src/getopt/genoptions.ggo_profile")
  else()
    set(GGO_FILE "${CMAKE_SOURCE_DIR}/src/getopt/genoptions.ggo_release")
  endif()
endif()

add_custom_command(
  OUTPUT "${GETOPT_DIR}/genoptions.c" "${GETOPT_DIR}/genoptions.h"
  COMMAND "${GENGETOPT}" -i"${GGO_FILE}" -Fgenoptions --unamed-opts
  WORKING_DIRECTORY "${GETOPT_DIR}"
  DEPENDS "${GGO_FILE}"
  COMMENT "Generating gengetopt sources"
)

# Core sources (match legacy Makefile subdirs; avoid experimental/unused code)
file(GLOB TRP_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cc"
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/src/stl_tim/*.cc"
  "${CMAKE_SOURCE_DIR}/src/adaptor/*.cc"
  "${CMAKE_SOURCE_DIR}/src/builder/*.cc"
  "${CMAKE_SOURCE_DIR}/src/prover/*.cc"
  "${CMAKE_SOURCE_DIR}/src/getopt/*.cc"
  "${CMAKE_SOURCE_DIR}/src/misc/*.cc"
)

list(REMOVE_ITEM TRP_SOURCES
  "${CMAKE_SOURCE_DIR}/src/tools/realpath.c"
)

# Avoid compiling generated headers as sources.
set(TRP_BISON_SRCS ${BISON_trp_parser_OUTPUTS})
list(FILTER TRP_BISON_SRCS EXCLUDE REGEX "\\.h$")

add_executable(trp
  ${TRP_SOURCES}
  ${TRP_BISON_SRCS}
  ${FLEX_trp_scanner_OUTPUTS}
  "${GETOPT_DIR}/genoptions.c"
)

# The parser and scanner sources include C++ code; compile them as C++.
set_source_files_properties(
  ${BISON_trp_parser_OUTPUTS}
  ${FLEX_trp_scanner_OUTPUTS}
  PROPERTIES LANGUAGE CXX
)

# Silence warnings from generated parser/scanner sources.
set_source_files_properties(
  ${BISON_trp_parser_OUTPUTS}
  ${FLEX_trp_scanner_OUTPUTS}
  PROPERTIES COMPILE_OPTIONS "-Wno-writable-strings;-Wno-unused-function"
)

set_target_properties(trp PROPERTIES
  OUTPUT_NAME "${TRP_PROGNAME}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_PROFILE "${CMAKE_SOURCE_DIR}/bin"
)

target_include_directories(trp PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${GEN_DIR}"
  "${GETOPT_DIR}"
  ${Boost_INCLUDE_DIRS}
)

if(APPLE)
  # Work around missing libc++ includes in some CLT setups.
  target_include_directories(trp SYSTEM PRIVATE
    "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1"
  )
endif()

set(TRP_EXTRA_INCLUDE_DIR "" CACHE PATH "Extra include directory")
if(TRP_EXTRA_INCLUDE_DIR)
  target_include_directories(trp PRIVATE "${TRP_EXTRA_INCLUDE_DIR}")
endif()

# Common compile definitions and flags
target_compile_options(trp PRIVATE -Wall)

target_compile_definitions(trp PRIVATE
  VERSION="${TRP_VERSION}"
  $<$<CONFIG:Debug>:DEBUG>
)

# Build the legacy realpath helper as a standalone tool (optional)
add_executable(realpath_tool "${CMAKE_SOURCE_DIR}/src/tools/realpath.c")
set_target_properties(realpath_tool PROPERTIES OUTPUT_NAME realpath)

# Optionally strip Release builds
option(TRP_STRIP_RELEASE "Strip Release binaries" ON)
if(TRP_STRIP_RELEASE)
  find_program(STRIP_TOOL strip)
  if(STRIP_TOOL)
    add_custom_command(TARGET trp POST_BUILD
      COMMAND $<$<CONFIG:Release>:${STRIP_TOOL}> $<$<CONFIG:Release>:$<TARGET_FILE:trp>>
      COMMENT "Stripping Release binary"
    )
  endif()
endif()
