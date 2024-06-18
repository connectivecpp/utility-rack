/** @file
 *
 * @brief Test scenarios for @c cast_ptr_to utility functions.
 *
 * @author Thurman Gillespy, Cliff Green
 *
 * @copyright (c) 2019-2024 by Thurman Gillespy, Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <cstddef> // std::byte
#include <cstdint> // std::uint16_t, std::uint32_t

#include "catch2/catch_test_macros.hpp"

#include "utility/cast_ptr_to.hpp"

// the following is a workaround for macOS / clang builds
void compare_bytes (std::byte b1, std::byte b2) noexcept {
  REQUIRE (std::to_integer<int>(b1) == std::to_integer<int>(b2));
}

TEST_CASE ( "casting unsigned int* to std::byte*", "[cast_ptr_to]" ) {

  SECTION ( "casting std::uint16_t* to const std::byte*" ) {
    std::uint16_t x = 0xAABB;
    const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    compare_bytes (*(p + 0), std::byte(0xBB));
    compare_bytes (*(p + 1), std::byte(0xAA));
  }

  SECTION ( "Casting the address of a 4 byte unsigned int to a const pointer" ) {
    std::uint32_t x = 0xAABBCCDD;
    const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    compare_bytes (*(p + 0), std::byte(0xDD));
    compare_bytes (*(p + 1), std::byte(0xCC));
    compare_bytes (*(p + 2), std::byte(0xBB));
    compare_bytes (*(p + 3), std::byte(0xAA));
  }

  SECTION ("Casting the address of a 4 byte signed int to a non-const pointer") {
    std::int32_t x = 0xDEADBEEF;
    std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    compare_bytes (*(p + 0), std::byte(0xEF));
    compare_bytes (*(p + 1), std::byte(0xBE));
    compare_bytes (*(p + 2), std::byte(0xAD));
    compare_bytes (*(p + 3), std::byte(0xDE));
  }
  SECTION ("Casting the pointer type to std::byte and back for the same type") {
    std::int32_t x = 0xDEADBEEF;
    const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    std::int32_t val;
    val = *(chops::cast_ptr_to<std::int32_t>(p));
    REQUIRE (val == x);
  }
}
