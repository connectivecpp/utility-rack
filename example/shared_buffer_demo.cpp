/** @file
 *  
 *  @brief Example code demonstrating use of @c chops::shared_buffer and
 *  @c chops::repeat. 
 * 
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/18/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file: 
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ shared_buffer_demo.cpp
 * 
 */

#include <iostream>
#include <cstdlib> // EXIT_SUCCESS
#include <functional>
#include <arpa/inet.h> // htons, ntohs

#include "utility/shared_buffer.hpp"
#include "utility/repeat.hpp"

// tasty utility lambda function
constexpr auto printLn = [] () { std::cout << std::endl; };

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
    const char* byte = reinterpret_cast<char*> (buf1.data()); // data starts here
    for (int i = 0; i < static_cast<int> (buf1.size()); i++) {
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
    std::cout << reinterpret_cast<char*> (buf1.data()) << std::endl;

    
    // write some short ints to a buffer
    constexpr int NUM_INTS = 15;
    chops::mutable_shared_buffer buf2(NUM_INTS * sizeof(std::uint16_t));
    std::cout << "buffer2 contains " << buf2.size() << " bytes and ";
    std::cout << (buf2.size()/sizeof(std::uint16_t)) << " short integers\n";
    // input some numbers using chops::repeat
    // data starts here
    std::uint16_t* valptr = reinterpret_cast<std::uint16_t*> (buf2.data()); 
    // create number, convert to 'network' byte order, place into buf2
    int count = 1;
    chops::repeat(NUM_INTS, [&] () {*valptr++ = htons(count++ * 5); });

    // now print them out
    valptr = reinterpret_cast<std::uint16_t*> (buf2.data());
    // read 2 bytes, convert back to proper endian order, print
    auto f = [&valptr] () { std::cout << ntohs(*valptr++) << " "; }; 
    chops::repeat(NUM_INTS, f);
    printLn();

    // swap the buffers, print result
    buf2.swap(buf1);
    std::cout << "buffer2 contents after swap" << std::endl;
    std::cout << reinterpret_cast<char*> (buf2.data()) << std::endl;
    std::cout << "buffer1 contents after swap" << std::endl;
    valptr = reinterpret_cast<std::uint16_t*> (buf1.data());
    chops::repeat(NUM_INTS, f); 
    printLn();

    return EXIT_SUCCESS;
}