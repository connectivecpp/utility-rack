# Utility Rack Release Status

## Release 1.0

Release 1.0 is under development (as of January 2020) and expected to be available by first half 2020. The functionality is already well tested under g++ and clang++ on Linux, MacOS, and Raspbian, but needs additional testing on other compilers and platforms.

## Next Steps, ToDo's, Problems, and Constraints:

- The CMake config files (CMakeLists.txts, etc) appear to be working for initial functionality, but enhancements and improvements are expected. In particular, dependency management through `find_package` can be improved, as well as options added for sanitizers and optimized builds.
- VC++ (and possibly Xcode) compiles (with the latest C++ standard flags) are needed (some preliminary compiles have already been performed, but CMake support will provide an easier way to compile and test with multiple build options). Compiling and building and testing on Windows 10 in addition to Linux and macOS and Raspberry Pi is also expected to be performed once CMake support is present.
- Code coverage tools have not been used on the codebase.

