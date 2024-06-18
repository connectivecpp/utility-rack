/** @file
 *
 * @brief Test scenarios for Blitz Rakete's utility function.
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

#include "utility/make_byte_array.hpp"
#include "utility/repeat.hpp"

// the following is a workaround for Catch2 link failures on macOS / clang builds
void compare_bytes (std::byte b1, std::byte b2) noexcept {
  REQUIRE (std::to_integer<int>(b1) == std::to_integer<int>(b2));
}

TEST_CASE ( "Blitz Rakete's utility function conveniently creates a std::array of std::bytes", 
            "[make_byte_array]" ) {
  SECTION ( "Various integers to put into a std::array of std::bytes" ) {
    auto arr = chops::make_byte_array(0x36, 0xd0, 0x42, 0xbe, 0xef);
    REQUIRE (arr.size() == 5);
    compare_bytes (arr[0], std::byte{0x36});
    compare_bytes (arr[1], std::byte{0xd0});
    compare_bytes (arr[2], std::byte{0x42});
    compare_bytes (arr[3], std::byte{0xbe});
    compare_bytes (arr[4], std::byte{0xef});
  }
  SECTION ( "The function is called with eleven arguments" ) {
    constexpr int N = 11;
    auto arr = chops::make_byte_array(
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
    );
    REQUIRE (arr.size() == N);
    chops::repeat(N, [&arr] (int i) { compare_bytes (arr[i], std::byte{0x11}); } );
  }
}
