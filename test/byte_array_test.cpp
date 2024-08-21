/** @file
 *
 * @brief Test scenarios for @c make_byte_array and @c compare_byte_arrays.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2018-2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch_test_macros.hpp"

#include "utility/byte_array.hpp"
#include "utility/repeat.hpp"

TEST_CASE ( "Compare two byte arrays", "[compare_byte_arrays]" ) {
  auto arr1 = chops::make_byte_array(0x10, 0x11, 0x12);
  auto arr2 {arr1};
  REQUIRE (chops::compare_byte_arrays(arr1, arr2));
  std::array<std::byte, 0> arr3;
  std::array<std::byte, 0> arr4;
  REQUIRE (chops::compare_byte_arrays(arr3, arr4));
}

TEST_CASE ( "Blitz Rakete's utility function conveniently creates a std::array of std::bytes", 
            "[make_byte_array]" ) {
  SECTION ( "Various integers to put into a std::array of std::bytes" ) {
    auto arr1= chops::make_byte_array(0x36, 0xd0, 0x42, 0xbe, 0xef);
    REQUIRE (arr1.size() == 5);
    std::array<std::byte, 5> arr2 { std::byte{0x36}, std::byte{0xd0},
                   std::byte{0x42}, std::byte{0xbe}, std::byte{0xef} };
    REQUIRE (chops::compare_byte_arrays(arr1, arr2));
  }
  SECTION ( "The function is called with eleven arguments" ) {
    constexpr int N = 11;
    auto arr = chops::make_byte_array(
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
    );
    REQUIRE (arr.size() == N);
    chops::repeat(N, [&arr] (int i) { REQUIRE (std::to_integer(arr[i]) == 0x11); } );
  }
}
