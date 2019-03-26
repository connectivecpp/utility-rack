## Example Programs

Example programs were compiled and tested on g++ 7.3.0 and clang 6.0.1 on Ubuntu 18.04.

g++ settings were:
`g++ -std=c++17 -Wall -Werror -g3 -fsanitize=address`

### erase_where_demo.cpp
It's a common mistake to forget to `erase` an elment from a container after calling `remove`. `chops::erase_where` combines remove, followed by erase.

### repeat_demo.cpp

### shared_buffer_demo.cpp

### threaded_queue_buffer_demo.cpp

### timer_demo.cpp