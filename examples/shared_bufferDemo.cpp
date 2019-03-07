/** @file
 *  
 *  @brief Example code demonstrating use of @c chops::shared_buffer and @c chops::repeat. 
 * 
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2018 by Thurman Gillespy
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#include <iostream>

#include "utility/shared_buffer.hpp"
#include "utility/repeat.hpp"

// tasty utility lambda function
auto printLn = [] () { std::cout << std::endl; };

int main() {
    
    // create empty shared buffer1
    chops::mutable_shared_buffer buf1;

    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    // c-string to add to buffer1
    char str1[] = "A cat in the hat.";
    char* strptr = str1;

    // add one char at a time, inside chops::repeat
    chops::repeat(sizeof(str1), [&] () { buf1.append((std::byte)*strptr++); });
    
    // what str1 and chops::repeat replaces
    // buf1.append((std::byte)'A');
    // buf1.append((std::byte)' ');
    // buf1.append((std::byte)'c');
    // etc

    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    // print the output, one char at a time
    char* byte = (char*)buf1.data(); // data starts here
    for (int i = 0; i < buf1.size(); i++) {
        std::cout << *byte++;
    }
    printLn();

    // append a string with one call to append
    buf1.clear(); // empty the buffer
    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    std::string str = "Green eggs and ham.";
    // convert str to C-string, add to buffer1
    buf1.append(str.c_str(), str.size()); 
    std::cout << "buffer1 contains " << buf1.size() << " bytes" << std::endl;
    std::cout << (char*)buf1.data() << std::endl; // print C-string

    // write some short ints to a preset size buffer
    int num_ints = 15;
    chops::mutable_shared_buffer buf2(num_ints * sizeof(short int));
    std::cout << "buffer2 contains " << buf2.size() << " bytes and ";
    std::cout << (buf2.size()/sizeof(short int)) << " short integers" << std::endl;
    // input some numbers using chops::repeat
    short int* valptr = (short int*)buf2.data(); // data starts here
    int count = 1;
    chops::repeat(num_ints, [&] () { *valptr++ = count++ * 5; });

    // now print them out
    // handy lambda print function
    auto f = [&] () { std::cout << *valptr++ << " "; }; 
    valptr = (short int*)buf2.data();
    chops::repeat(num_ints, f);
    printLn();

    // swap the buffers, print result
    buf2.swap(buf1);
    std::cout << "buffer2 contents after swap" << std::endl;
    std::cout << (char*)buf2.data() << std::endl;
    std::cout << "buffer1 contents after swap" << std::endl;
    valptr = (short int*)buf1.data();
    chops::repeat(num_ints, f);
    printLn();

    return EXIT_SUCCESS;
}