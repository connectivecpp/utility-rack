## Utility Rack Overview

### Repeat                                                                                                                                                                                                                                            
Repeat is a function template to abstract and simplify loops that repeat N times, from Vittorio Romeo. The C++ range based `for` doesn't directly allow N repetitions of code. Vittorio's utility fills that gap. Vittorio's blog article is [here](https://vittorioromeo.info/index/blog/abstraction_design_implementation_repeat.html).

### Erase Where

A common mistake in C++ is to forget to call `std::erase` after calling `std::remove`. This utility wraps the two together allowing either a value to be directly removed from a container, or a set of values to be removed using a predicate. This utility code is copied from a StackOverflow post by Richard Hodges (see [References](https://connectivecpp.github.io/doc/references.html)).

### Byte Array

Since `std::byte` pointers are used as a general buffer interface, a small utility function from Blitz Rakete as posted on Stackoverflow (see [References](https://connectivecpp.github.io/doc/references.html)) is useful to simplify creation of byte buffers, specially for testing purposes. In addition, a utility function to compare `std::byte` arrays is provided.

### Cast Pointer To

__Note__: This utility is mostly or completely outdated due to the C++ 20 `std::bit_cast` function.

Using `reinterpret_cast` in C++ may mean undefined (even if well understood and executionally correct) behavior. In networking and other types of I/O processing it is a common need to convert a pointer to a `char *`, or in C++ 17 (and later) a `std::byte *`.

The `cast_ptr_to` utility conveniently combines a static cast to `void *` with a static cast to a specified (via function template parameter) type. Typically the destination type is a `std::byte` pointer.

If the destination type is unrelated to the original type (and is not a `void *` or some form of `char *` such as `std::byte *`) __undefined behavior__ will still occur. However, if converting the pointer to a `std::byte` pointer and then back to the original pointer type, the behavior is well defined and safe.

### Overloaded

This utility creates a class providing a set of function object overloads (`operator()`) from a parameter pack. There is both a class template and a function template. This utility is specially useful when calling `std::visit`, allowing a set of lambdas to be created corresponding to the visitation set for a `std::variant`. The code is directly copied from [C++ Reference](https://en.cppreference.com/w/cpp/utility/variant/visit).

### Forward Capture

Capturing perfectly forwarded references in a lambda is difficult. (Forwarding references are also called universal references, a term coined by Scott Meyers.) This utility eases the task with a level of indirection. The design and code come from Vittorio Romeo's [blog article](https://vittorioromeo.info/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html).

