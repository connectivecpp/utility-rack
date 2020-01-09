/** @file
 *  
 *  @brief Example code demonstrating use of @c chops::shared_buffer and
 *  @c chops::repeat. See @c threaded_wait_shared_demo.cpp for multithreaded
 *  example.
 * 
 *  @ingroup example_module
 *
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/22/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file: 
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ \
 *  -I ~/Projects/boost_1_69_0/ \
 *  shared_buffer_demo.cpp
 *  
 * 
 */

#include <iostream>
#include <cstdlib> // EXIT_SUCCESS
#include <cstddef> // std::byte
#include <cstdint> // std::uint16_t
#include <string>

#include "marshall/extract_append.hpp"
#include "marshall/shared_buffer.hpp"
#include "utility/repeat.hpp"

// tasty utility lambda function
constexpr auto printLn = [] () { std::cout << std::endl; };

// utility functions for casting @c wait_queue.data()
template <class C>
const char* cast_to_char_ptr (const C& buf) {
    return static_cast<const char*> (static_cast<const void*> (buf.data()));
}

template <class C>
const std::uint16_t* cast_to_uint16_ptr (const C& buf) {
    return static_cast<const std::uint16_t*> (static_cast<const void*> (buf.data()));
}


int main() {
    
    // create empty shared buffer1
    chops::mutable_shared_buffer buf1;

    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    // c-string to add to buffer1
    constexpr char str1[] = "A cat in the hat.";
    const char* strptr = str1;

    // add one char at a time, inside chops::repeat
    chops::repeat(sizeof(str1),
        [&] () { buf1.append(static_cast<std::byte> (*strptr++)); });
    
    // what str1 and chops::repeat replaces
    // buf1.append(static_cast<std::byte>('A');
    // buf1.append((static_cast<std::byte>(' ');
    // buf1.append((static_cast<std::byte>('c');
    // etc. 

    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    // print the output, one char at a time
    const char* byte = cast_to_char_ptr (buf1); // data starts here
    for (unsigned int i = 0; i <  buf1.size(); ++i) {
        std::cout << *byte++;
    }
    printLn();

    // append a string with one call to append
    buf1.clear(); // empty the buffer
    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    const std::string str = "Green eggs and ham.";
    // convert str to C-string, add to buffer1
    buf1.append(str.c_str(), str.size() + 1); 
    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    // print c-string
    std::cout << cast_to_char_ptr (buf1) << std::endl;

    
    // write some short ints to a buffer
    constexpr int NUM_INTS = 15;
    chops::mutable_shared_buffer buf2(NUM_INTS * sizeof(std::uint16_t));
    std::cout << "buffer2 contains " << buf2.size() << " bytes and ";
    std::cout << (buf2.size()/sizeof(std::uint16_t)) << " short integers\n";

    // input some numbers using chops::repeat
    const std::uint16_t* data = cast_to_uint16_ptr (buf2);// data starts here
    std::uint16_t* valptr = const_cast<std::uint16_t*>(data); // remove const*

    // create number, convert to 'network' (big endian) byte order, place into buf2
    std::uint16_t count = 1;
    chops::repeat(NUM_INTS, [count, x = buf2.data()] () mutable {auto sz =
        chops::append_val <std::uint16_t> (x, count++ * 5); x += sz; });

    // print them out
    valptr = const_cast<std::uint16_t*> (data);
    // read 2 bytes, convert back to proper endian order, print
    auto f = [x = buf2.data()] () mutable { std::cout << chops::extract_val<std::uint16_t>(x) << " "; x+=2;};
    chops::repeat(NUM_INTS, f);
    printLn();

    // swap the buffers, print result
    buf2.swap(buf1);
    std::cout << "buffer2 contents after swap" << std::endl;
    std::cout << cast_to_char_ptr (buf2) << std::endl;
    std::cout << "buffer1 contents after swap" << std::endl;
    valptr = const_cast<std::uint16_t*> (data);
    chops::repeat(NUM_INTS, f); 
    printLn();

    return EXIT_SUCCESS;
}
