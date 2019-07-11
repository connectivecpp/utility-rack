# Utility Rack Release Status

## Release 1.0

Release 1.0 is under development (as of July 2019) and expected to be available by late Summer 2019. The functionality is already well tested under g++ and Linux, but needs additional testing on other compilers and platforms.

## Next Steps, ToDo's, Problems, and Constraints:

- The CMake config files (CMakeLists.txts, etc) appear to be working for initial functionality, but enhancements and improvements are expected. In particular, dependency management through `find_package` can be improved, as well as options added for sanitizers and optimized builds.
- VC++ (and possibly Xcode) compiles (with the latest C++ standard flags) are needed (some preliminary compiles have already been performed, but CMake support will provide an easier way to compile and test with multiple build options). Compiling and building and testing on Windows 10 in addition to Linux and macOS and Raspberry Pi is also expected to be performed once CMake support is present.
- GitHub continuous integration procedures and config files (e.g. Jenkins and Travis) need to be created and tested.
- The code is well Doxygenated and high level descriptions are present. However, HTML needs to be generated from the Doxygen, or possibly a "Doxygen to markdown" process put in place.
- Code coverage tools have not been used on the codebase.

