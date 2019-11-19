 Branch | Status |
 | --- | --- |
| **Master** | [![Build Status](https://travis-ci.org/connectivecpp/utility-rack.svg?branch=master)](https://travis-ci.org/connectivecpp/utility-rack)
| **Develop** | [![Build Status](https://travis-ci.org/connectivecpp/utility-rack.svg?branch=develop)](https://travis-ci.org/connectivecpp/utility-rack)


# Utility Rack - Tasty, Useful C++ Classes and Functions

Th C++ classes and functions in this repository are designed for general purpose use. In addition, many of them are used in the Chops ("C"onnective "H"andcrafted "Op"enwork "S"oftware) libraries (e.g. Chops Net IP, an asynchronous IP networking library). The Chops libraries provide networking and distributed processing functionality and are specially useful for efficiently connecting multiple types of devices together.

This software is written using modern C++ design idioms and the latest (2017) C++ standard.

# License

This project is distributed under the [Boost Software License](LICENSE.txt).

[![Licence](https://img.shields.io/badge/license-boost-4480cc.png)](http://www.boost.org/LICENSE_1_0.txt)

## Utility Rack Release Status

Release 1.0 is under development as of October 2019, awaiting CMake config file completion and testing under multiple compilers and platforms.

Release notes and upcoming development plans are [available here](doc/release.md).

# Utility Rack Components

## Marshall Utilities

### Marshall

The marshall functions and classes provide marshalling and unmarshalling of binary data. Marshalling (also called serialization, depending on context) provides a way to transform application objects into and out of byte streams that can be sent over a network (or use for file IO). 

This marshalling functionality in this repository is useful when explicit control is needed for every bit and byte. This allows a developer to match an existing wire protocol or encoding scheme or to define his or her own wire protocol. Support is provided for fundamental arithmetic types as well as certain C++ vocabulary types such as @c std::optional. The binary data is always marshalled to big-endian (network) format.

### Shared Buffer

Reference counted byte buffer classes are used within the marshalling classes as well as the Chops Net IP library, but can be useful in other contexts. These classes are based on example code inside Chris Kohlhoff's Asio library (see [References](doc/references.md)). 

A detailed overview of the marshall classes is [available here](doc/marshall.md).

## Queue Utilities

### Wait Queue

Wait Queue is a multi-reader, multi-writer FIFO queue for transferring data between threads. It is templatized on the type of data passed through the queue as well as the queue container type. Data is passed with value semantics, either by copying or by moving (as opposed to a queue that transfers data by pointer or reference). The wait queue has both wait and no-wait pop semantics, as well as simple "close" and "open" capabilities (to allow graceful shutdown or restart of thread or process communication). A fixed size container (e.g. a `ring_span`) can be used, eliminating any and all dynamic memory management (useful in embedded or performance constrained environments). Similarly, a circular buffer that only allocates on construction can be used, which eliminates dynamic memory management when pushing or popping values on or off the queue.

Wait Queue is inspired by code from Anthony Williams' Concurrency in Action book (see [References](doc/references.md)), although heavily modified.

A detailed overview is [available here](doc/queue.md).

## Timer Utilities

### Periodic Timer

The Periodic Timer class is an asynchronous periodic timer that wraps and simplifies Asio timers (see [References](doc/references.md)) when periodic callbacks are needed. The periodicity can be based on either a simple duration or on timepoints based on a duration.

Asynchronous timers from Asio are relatively easy to use. However, there are no timers that are periodic. This class simplifies the usage, using application supplied function object callbacks. When the timer is started, the application specifies whether each callback is invoked based on a duration (e.g. one second after the last callback), or on timepoints (e.g. a callback will be invoked each second according to the clock).

A detailed overview is [available here](doc/timer.md).

## General Utilities

### Repeat

Repeat is a function template to abstract and simplify loops that repeat N times, from Vittorio Romeo (see [References](doc/references.md)). The C++ range based `for` doesn't directly allow N repetitions of code. Vittorio's utility fills that gap.

### Erase Where

A common mistake in C++ is to forget to call `std::erase` after calling `std::remove`. This utility wraps the two together allowing either a value to be directly removed from a container, or a set of values to be removed using a predicate. This utility code is copied from a StackOverflow post by Richard Hodges (see [References](doc/references.md)).

### Make Byte Array

Since `std::byte` pointers are used as a general buffer interface, a small utility function from Blitz Rakete as posted on Stackoverflow (see [References](doc/references.md)) is useful to simplify creation of byte buffers, specially for testing purposes.

### Cast Pointer To

Using `reinterpret_cast` in C++ may mean undefined (even if well understood and executionally correct) behavior. In networking and other types of I/O processing it is a common need to convert a pointer to a `char *`, or in C++ 17 (and later) a `std::byte *`.

The `cast_ptr_to` utility conveniently combines a static cast to `void *` with a static cast to a specified (via function template parameter) type. Typically the destination type is a `std::byte` pointer.

If the destination type is unrelated to the original type (and is not a `void *` or some form of `char *` such as `std::byte *`) undefined behavior will still occur. However, if converting the pointer to a `std::byte` pointer and then back to the original pointer type, the behavior is well defined and safe.

### Overloaded

This utility creates a class providing a set of function object overloads (`operator()`) from a parameter pack. There is both a class template and a function template. This utility is specially useful when calling `std::visit`, allowing a set of lambdas to be created corresponding to the visitation set for a `std::variant`. The code is directly copied from https://en.cppreference.com/w/cpp/utility/variant/visit.

### Forward Capture

Capturing perfectly forwarded references in a lambda is difficult. (Forwarding references are also called universal references, a term coined by Scott Meyers.) This utility eases the task with a level of indirection. The design and code come from Vittorio Romeo's blog (see [References](doc/references.md)).

# C++ Language Requirements and Alternatives

C++ 17 is the primary baseline for this repository.

A significant number of C++ 11 features are in the implementation and API. There are also numerous C++ 14 and C++ 17 features in use, although many of them could be replaced with Boost (or similar) utilities or rewritten to use only C++ 11 capabilities. For users that don't want to use the latest C++ compilers or compile with C++ 17 flags, Martin Moene provides an excellent set of header-only libraries that implement many useful C++ library features, both C++ 17 as well as future C++ standards (see [References](doc/references.md)).

While the main production branch will always be developed and tested with C++ 17 features (and relatively current compilers), alternative branches and forks for older compiler versions are expected. In particular, a branch using Martin Moene's libraries and general C++ 11 (or C++ 14) conformance is likely. Collaboration (through forking, change requests, etc) is very welcome to achieve older compiler conformance. A branch supporting a pre-C++ 11 compiler or language conformance is not likely to be directly supported through this repository (since it would require so many changes that it would result in a defacto different codebase).

# External Dependencies

The libraries and API's have minimal (as possible) library dependencies (there are heavy dependencies on the C++ standard library in all of the code). There are more dependencies in the test code than in the production code.

All the dependencies listed below have links that will take you to the library or repository.

Production external dependencies:

- Version 1.13 (or later) of Chris Kohlhoff's [`asio`](https://github.com/chriskohlhoff/asio) library is required. Note that it is the stand-alone library, not the Boost Asio implementation.

Test external dependencies:

- Version 2.8.0 (or later) of Phil Nash's [`Catch2`](https://github.com/catchorg/Catch2) library is required for all test scenarios.
- Version 0.3 (or later) of Martin Moene's [`ring-span-lite`](https://github.com/martinmoene/ring-span-lite) library is required in some test scenarios.
- Version Thu Jul 12 21:58:58 2018 +0300 (or later) of Justas Masiulis [`circular_buffer`](https://github.com/JustasMasiulis/circular_buffer) library is required in some test scenarios (see specifics below; this dependency may be copied into the `utility-rack` repository in the future).


See [References](doc/references.md) for additional details on the above libraries.

Specific dependencies:

- All test scenarios: Catch 2
- Periodic Timer (production): `asio`
- Wait Queue (production): none
    - Wait Queue (test): `ring-span-lite`
    - Wait Queue (test): `circular-buffer`

# Supported Compilers and Platforms

Utility Rack has been compiled and tests run on:


- g++ 7.2, g++ 7.3, Linux (Ubuntu 17.10 - kernel 4.13, Ubuntu 18.04 - kernel 4.15)
- (TBD, will include at least clang on linux and vc++ on Windows)

# Installation

The Utility Rack components are header-only, so installation consists of downloading or cloning and setting compiler include paths appropriately. No compile time configuration macros are defined.

# References

See [References](doc/references.md) for details on dependencies and inspirations for Utility Rack.

# About

The primary author of Utility Rack is Cliff Green, cliffg at connectivecpp dot com. The primary co-authors are Thurman Gillespy, thurmang at connectivecpp dot com, and Roxanne Agerone, roxanne at connectivecpp dot com.

Contributors include Matthew Briggs, Daniel Muldrew, and Bob Higgins.

Additional information including author comments is [available here](doc/about.md).

