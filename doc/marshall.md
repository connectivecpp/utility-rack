# Marshall Detailed Overview

## Marshall

- Has been tested with 

- Does not throw or catch exceptions anywhere in its code base. Elements passed through the queue may throw exceptions, which must be handled at an application level. Exceptions may be thrown by C++ std library concurrency calls (`std::mutex` locks, etc), although this usually indicates an application design issue or issues at the operating system level.

- If the C++ std library concurrency calls become `noexcept`, every Wait Queue method will become `noexcept` or conditionally `noexcept` (depending on the type of the data passed through the Wait Queue).

The only requirement on the type passed through a Wait Queue is that it supports either copy construction or move construction. In particular, a default constructor is not required (this is enabled by using `std::optional`, which does not require a default constructor).

The implementation is adapted from the book Concurrency in Action, Practical Multithreading, by Anthony Williams (see [References Section](../README.md#references)). 

The core logic in this library is the same as provided by Anthony in his book, but the API has changed and additional features added. The name of the utility class template in Anthony's book is `threadsafe_queue`.


