# Utility Rack - Tasty, Useful C++ Classes and Functions

#### Unit Test and Documentation Generation Workflow Status

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/utility-rack/build_run_unit_test_cmake.yml?branch=main&label=GH%20Actions%20build,%20unit%20tests%20on%20main)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/utility-rack/build_run_unit_test_cmake.yml?branch=develop&label=GH%20Actions%20build,%20unit%20tests%20on%20develop)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/utility-rack/gen_docs.yml?branch=main&label=GH%20Actions%20generate%20docs)

## Overview

The C++ classes and functions in this repository are designed for general purpose use. In addition, many of them are used in the Chops ("C"onnective "H"andcrafted "Op"enwork "S"oftware) libraries (e.g. Chops Net IP, an asynchronous IP networking library). The Chops libraries provide networking and distributed processing functionality and are specially useful for efficiently connecting multiple types of devices together.

This software is written using modern C++ design idioms and the C++ 20 standard.

## Generated Documentation

The generated Doxygen documentation for these utilities is [here](https://connectivecpp.github.io/utility-rack/).

## Dependencies

The `wait_queue` header file does not have any third-party dependencies. It uses C++ standard library headers only. The unit test code does have dependencies as noted below.

## C++ Standard

`wait_queue`  uses C++ 20 features, including `std::stop_token`, `std::stop_source`, `std::condition_variable_any`, `std::scoped_lock`, and `concepts` / `requires`.

## Supported Compilers

Continuous integration workflows build and unit test on g++ (through Ubuntu) and MSVC (through Windows). Note that clang support for C++ 20 `std::jthread` and `std::stop_token` is still experimental (and possibly incomplete) as of May 2024, so has not (yet) been tested with `wait_queue`.

## Unit Test Dependencies

The unit test code uses [Catch2](https://github.com/catchorg/Catch2). If the `WAIT_QUEUE_BUILD_TESTS` flag is provided to Cmake (see commands below) the Cmake configure / generate will download the Catch2 library as appropriate using the [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) dependency manager. If Catch2 (v3 or greater) is already installed using a different package manager (such as Conan or vcpkg), the `CPM_USE_LOCAL_PACKAGES` variable can be set which results in `find_package` being attempted. Note that v3 (or later) of Catch2 is required.

The unit test uses two third-party libraries (each is a single header-only file):

- Martin Moene's [ring-span-lite](https://github.com/martinmoene/ring-span-lite)
- Justas Masiulis' [circular_buffer](https://github.com/JustasMasiulis/circular_buffer)

Specific version (or branch) specs for the dependencies are in `test/CMakeLists.txt`.

## Build and Run Unit Tests

To build and run the unit test program:

First clone the `utility-rack` repository, then create a build directory in parallel to the `utility-rack` directory (this is called "out of source" builds, which is recommended), then `cd` (change directory) into the build directory. The CMake commands:

```
cmake -D WAIT_QUEUE_BUILD_TESTS:BOOL=ON -D JM_CIRCULAR_BUFFER_BUILD_TESTS:BOOL=OFF ../utility-rack

cmake --build .

ctest
```

For additional test output, run the unit test individually, for example:

```
test/wait_queue_test -s
```

The example can be built by adding `-D WAIT_QUEUE_BUILD_EXAMPLES:BOOL=ON` to the CMake configure / generate step.

