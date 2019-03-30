/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for @c cast_ptr_to utility functions.
 *
 *  @author Thurman Gillespy, Cliff Green
 *
 *  Copyright (c) 2019 by Thurman Gillespy, Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <cstddef> // std::byte
#include <cstdint> // std::uint16_t, std::uint32_t

#include "catch2/catch.hpp"
#include "utility/cast_ptr_to.hpp"

SCENARIO( "casting int* to std::byte*", "[cast_ptr_to]" ) {

  GIVEN ("Casting the address of a 2 byte int") {
    std::uint16_t x = 0xAABB;

    WHEN ("casting std::uint16_t* to std::byte*") {
    const std::byte* p = cast_ptr_to<std::byte> (&x);
      THEN ("the first byte should be 0xBB") {
        REQUIRE (*p == std::byte(0xBB));
      } AND_THEN ("the second byte should be 0xAA") {
        REQUIRE (*(p + 1) == std::byte(0xAA));
      }
    }
  } // end given

  GIVEN ("Casting the address of a 4 byte int") {
    std::uint32_t x = 0xAABBCCDD;

    WHEN ("casting std::uint32_t* to std::byte*") {
    const std::byte* p = cast_ptr_to<std::byte> (&x);
      THEN ("the bytes should be 0xDD, 0xCC, 0xBB, 0xAA") {
        REQUIRE (*p == std::byte(0xDD));
        REQUIRE (*(p + 1) == std::byte(0xCC));
        REQUIRE (*(p + 2) == std::byte(0xBB));
        REQUIRE (*(p + 3) == std::byte(0xAA));
      }
    }
  } // end given
}
