# Example Code for Presentations

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/presentations/build_run_unit_test_cmake.yml?branch=main&label=GH%20Actions%20build,%20unit%20tests%20on%20main)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/presentations/build_run_unit_test_cmake.yml?branch=develop&label=GH%20Actions%20build,%20unit%20tests%20on%20develop)

The generated Doxygen documentation for the example code is [here](https://connectivecpp.github.io/presentations/).

This example source code is taken from the presentations in the [presentations](https://github.com/connectivecpp/presentations/blob/main/presentations/) directory. The example code is in the [examples](http://github.com/connectivecpp/presentations/blob/main/examples/) directory.
                                                                                                                           This README file serves as both a typical README as well as the main page of the generated Doxygen HTML.

All of the example source code has [Catch2](https://github.com/catchorg/Catch2) unit test code. The Catch2 unit test presentation (of course) features the unit test code. Each directory corresponding to a presentation has a top level CMake file.

Each CMake configure / generate will download and build the Catch2 library as appropriate using the [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) dependency manager. If Catch2 (v3 or greater) is already installed using a different package manager (such as Conan or vcpkg), the `CPM_USE_LOCAL_PACKAGES` variable can be set which results in `find_package` being attempted. Note that v3 (or later) of Catch2 is required, which results in faster unit test build times (once the initial Catch2 library is compiled) due to Catch2 changing from a "header only" library to a library with compiled objects (in addition to the header files).

All of the example code builds and successfully runs (as unit tests) on Ubuntu (Linux), Windows, and macOS, using GitHub Actions (the GitHub continuous integration facilities). Specifically, `ubuntu-latest`, `windows-latest`, and `macos-latest` or `macos-14` are the target runners specified in the build matrix in the workflow YAML file.

All of the CMake files have C++ 20 set as the required C++ standard, but only small portions require C++ 20, so most of the code can be used with older C++ standards.

The `intro_generic_programming` example uses a third party `decimal` library from [Tim Quelch](https://github.com/TimQuelch/decimal). The CMake configure / generate step requires the `decimal` test code to be bypassed in the build (it uses an older version of Catch2) - see notes below for specifics.

To build and run (all of) the example test programs:

First clone the `presentations` repository, then create a build directory in parallel to the presentations directory (this is called "out of source" builds), then `cd` (change directory) into the build directory. The CMake commands:

```
cmake -D DECIMAL_ENABLE_TESTING:BOOL=OFF ../presentations/examples

cmake --build .

ctest
```

For additional test output, each test can be invoked individually, for example:

```
std_span/std_span_test -s
```

Currently [Doxygen](https://www.doxygen.nl/index.html) is used to extract and generate documentation from the example code. In the future, additional tools such as [Sphinx](https://www.sphinx-doc.org/) may be used. Sphinx provides a modern look and feel and additional capabilities to tie together tutorials and example code. Sphinx uses the [reStructuredText](https://docutils.sourceforge.io/rst.html) markup language.

