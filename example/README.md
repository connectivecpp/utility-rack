## Example Programs

Example programs were compiled and tested with g++ 7.3.0 and clang 6.0.1 on Ubuntu 18.04. g++ settings were:

`g++ -std=c++17 -Wall -Werror -g3 -fsanitize=address`

### erase_where_demo.cpp
It is a common mistake to forget to `erase` an elment from a container after calling `remove`. The demo illustrates how the container size is unchanged after `remove`, but decreases after `erase`. `chops::erase_where` combines the two operations.

### repeat_demo.cpp
A convenience function to repeat code N times. The lambda or function object can access the iteration count, if desired.

`chops::repeat` is also used in `shared_buffer_demo.cpp` and `threaded_queue_buffer_demo.cpp`.

### shared_buffer_demo.cpp
Demonstration of `chops::mutable_shared_buffer`, a modifiable `std::byte` buffer with convenience methods, and internally reference counting for efficient copying. The demo code has utility functions for casting the buffer `std::byte*` pointer to `char*` and `std::uint16_t*`. The code also shows how to use the `boost endian` facilities to handle endian byte order when the buffer is used over a network connection.

See `threaded_queue_buffer_demo.cpp` for how to use the `shared_buffer` in mutltithreaded code.

### threaded_queue_buffer_demo.cpp
Demo code showing use of `chops::wait_queue` in multithreaded environment. The program has 1...N data generators (default 20) that each place 20 random nubmers in a `wait_queue<int>`; each generator is in a separate thread. Then 1...N data processors (default 5) periodically convert the `int` data into `string`s that are placed into a `chops::wait_queue<chops::mutable_shared_buffer>>`. A single database thread puts the data from the second `wait_queue` into a simple database.

The demo program can take 2 command line arguments that set the number of data generators and data processors, respectively.

### timer_demo.cpp
Demo code showing utiltiy functions that create periodic timers from the `Asio` libraray.

### wait_queue_demo.cpp
Brief demonstation of using the `chops::wait_queue` API, with the queue semantics `chops::wait_queue::push` and `chops::wait_queue::try_pop`. The example also uses the `wait_queue::apply` method to iterate a non-modifying function over the queue elements. See `threaded_queue_buffer_demo.cpp` for a more in depth example using `wait_queue` in multithreaded code.