# Utility Rack Release Status

## Release 0.x

Release 0.x is under development (Aug, 2018). The functionality is well tested under g++ and Linux.

## Next Steps, ToDo's, Problems, and Constraints:

- This is a good point to ask for project help and collaboration, which will be greatly appreciated (for many reasons).
- The makefiles and build infrastructure components are not yet present. A working CMakeLists.txt is needed as well as Github continuous integration procedures (e.g. Jenkins and Travis).
- The code is well doxygenated, and there is a good start on the high level descriptions, but tutorials and other high-level documentation is needed. A "doxygen to markdown" procedure is needed (or an equivalent step to generate the documentation from the embedded doxygen).
- The code only compiles on one compiler, but VC++ and Clang support (with the latest C++ standard flags) is expected soon. Compiling and building on Windows 10 is also expected to be supported at that time. Once multiple compilers and desktop environments are tested, development will expand to smaller and more esoteric environments (e.g. Raspberry Pi).
- Code coverage tools have not been used on the codebase.

