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

TEST_CASE ( "casting unsigned int* to std::byte*", "[cast_ptr_to]" ) {

  SECTION ( "casting std::uint16_t* to const std::byte*" ) {
    std::uint16_t x = 0xAABB;
    const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    REQUIRE (*(p + 0) == std::byte(0xBB));
    REQUIRE (*(p + 1) == std::byte(0xAA));
  }

  SECTION ( "Casting the address of a 4 byte unsigned int to a const pointer" ) {
    std::uint32_t x = 0xAABBCCDD;
    const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    REQUIRE (*(p + 0) == std::byte(0xDD));
    REQUIRE (*(p + 1) == std::byte(0xCC));
    REQUIRE (*(p + 2) == std::byte(0xBB));
    REQUIRE (*(p + 3) == std::byte(0xAA));
  }

  SECTION ("Casting the address of a 4 byte signed int to a non-const pointer") {
    std::int32_t x = 0xDEADBEEF;
    std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    REQUIRE (*(p + 0) == std::byte(0xEF));
    REQUIRE (*(p + 1) == std::byte(0xBE));
    REQUIRE (*(p + 2) == std::byte(0xAD));
    REQUIRE (*(p + 3) == std::byte(0xDE));
  }
  SECTION ("Casting the pointer type to std::byte and back for the same type") {
    std::int32_t x = 0xDEADBEEF;
    const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
    std::int32_t val;
    val = *(chops::cast_ptr_to<std::int32_t>(p));
    REQUIRE (val == x);
  }
}
