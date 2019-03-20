/** @file
 *  
 *  @brief Example code demonstrating use of @c chops::erase_where and 
 *  @x chops::erase_where_if.
 * 
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/19/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file:
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ \
 *  erase_where_demo.cpp
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm> // std::remove, std::remove_if

#include "utility/erase_where.hpp"

/**
 *  @brief @c std::remove and @c std::remove_if move the found element(s) to the end of
 *  the container. The size of the collection does not chnange. @c erase must be 
 *  subsequently called to actually remove the elements from the container. 
 * 
 *  @c chops::erase_where and @c chops::erase_where_if are convenience functions that
 *  perform the erase-remove idiom.
 */
int main() {

    std::vector<int> arr {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto printSize = [] (std::vector<int>& arr) 
        {std::cout << "# elements in array: " << arr.size() << std::endl;};
    
    auto printValues = [] (std::vector<int>& arr) 
    { std::for_each(arr.begin(), arr.end(), [] (int i) { std::cout << i << " "; });
    std::cout << std::endl;};

    std::cout << "std::remove, followed by erase" << std::endl;

    printSize(arr);
    printValues(arr);

    // remove the 5
    auto it = std::remove(arr.begin(), arr.end(), 5);

    printSize(arr);
    printValues(arr);

    arr.erase(it, arr.end());
    printSize(arr);
    printValues(arr);

    // remove even numbers
    it = std::remove_if(arr.begin(), arr.end(), [] (int i) { return i % 2 == 0; });

    printSize(arr);
    printValues(arr);

    arr.erase(it, arr.end());

    printSize(arr);
    printValues(arr);

    // erase_where
    std::cout << "\nerase_where and erase_where_if" << std::endl;

    std::vector<int> arr2 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    printSize(arr2);
    printValues(arr2);

    chops::erase_where(arr2, 5);
    chops::erase_where_if(arr2, [] (int i) { return i % 2 == 0 ; });

    printSize(arr2);
    printValues(arr2);


    return EXIT_SUCCESS;
}

