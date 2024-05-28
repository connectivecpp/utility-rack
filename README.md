# Utility Rack - Tasty, Useful Header-Only C++ Classes and Functions

#### Unit Test and Documentation Generation Workflow Status

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/utility-rack/build_run_unit_test_cmake.yml?branch=main&label=GH%20Actions%20build,%20unit%20tests%20on%20main)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/utility-rack/build_run_unit_test_cmake.yml?branch=develop&label=GH%20Actions%20build,%20unit%20tests%20on%20develop)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/utility-rack/gen_docs.yml?branch=main&label=GH%20Actions%20generate%20docs)

![GH Tag](https://img.shields.io/github/v/tag/connectivecpp/utility-rack?label=GH%20tag)

## Overview

The C++ classes and functions in this repository are designed for general purpose use. In addition, many of them are used in the Chops ("C"onnective "H"andcrafted "Op"enwork "S"oftware) libraries (e.g. Chops Net IP, an asynchronous IP networking library). The Chops libraries provide networking and distributed processing functionality and are specially useful for efficiently connecting multiple types of devices together.

More info on the utilities is [here](include/utility/overview.md). It is also the main page of the generated Doxygen documentation.

## Generated Documentation

The generated Doxygen documentation for these utilities is [here](https://connectivecpp.github.io/utility-rack/).

## Dependencies

The utilities in `utility-rack` do not have any third-party dependencies. They use C++ standard library headers only. The unit test code does have dependencies as noted below.

## C++ Standard

This software is compiled with the C++ 20 standard, although some of the idioms were written for C++ 17 and may have newer and more modern implementations. C++ 20 features such as `concepts` / `requires` will be added.

## Supported Compilers

Continuous integration workflows build and unit test on g++ (through Ubuntu), MSVC (through Windows), and clang (through macOS).

## Unit Test Dependencies

The unit test code uses [Catch2](https://github.com/catchorg/Catch2). If the `UTILITY_RACK_BUILD_TESTS` flag is provided to Cmake (see commands below) the Cmake configure / generate will download the Catch2 library as appropriate using the [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) dependency manager. If Catch2 (v3 or greater) is already installed using a different package manager (such as Conan or vcpkg), the `CPM_USE_LOCAL_PACKAGES` variable can be set which results in `find_package` being attempted. Note that v3 (or later) of Catch2 is required.

Specific version (or branch) specs for the Catch2 dependency is in the [test/CMakeLists.txt](test/CMakeLists.txt) file, look for the `CPMAddPackage` command.

## Build and Run Unit Tests

To build and run the unit test program:

First clone the `utility-rack` repository, then create a build directory in parallel to the `utility-rack` directory (this is called "out of source" builds, which is recommended), then `cd` (change directory) into the build directory. The CMake commands:

```
cmake -D UTILITY_RACK_BUILD_TESTS:BOOL=ON ../utility-rack

cmake --build .

ctest
```

For additional test output, run the unit test individually, for example:

```
test/erase_where_test -s
test/make_byte_array_test -s
```

The example can be built by adding `-D UTILITY_RACK_BUILD_EXAMPLES:BOOL=ON` to the CMake configure / generate step.

