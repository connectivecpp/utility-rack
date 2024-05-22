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

#include "catch2/catch.hpp"
#include "utility/cast_ptr_to.hpp"

SCENARIO( "casting unsigned int* to std::byte*", "[cast_ptr_to]" ) {

  GIVEN ("Casting the address of a 2 byte unsigned int to a const pointer") {
    std::uint16_t x = 0xAABB;

    WHEN ("casting std::uint16_t* to const std::byte*") {
      const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
      THEN ("the first byte should be 0xBB") {
        REQUIRE (*(p + 0) == std::byte(0xBB));
      } AND_THEN ("the second byte should be 0xAA") {
        REQUIRE (*(p + 1) == std::byte(0xAA));
      }
    }
  } // end given

  GIVEN ("Casting the address of a 4 byte unsigned int to a const pointer") {
    std::uint32_t x = 0xAABBCCDD;

    WHEN ("casting std::uint32_t* to const std::byte*") {
      const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
      THEN ("the bytes should be 0xDD, 0xCC, 0xBB, 0xAA") {
        REQUIRE (*(p + 0) == std::byte(0xDD));
        REQUIRE (*(p + 1) == std::byte(0xCC));
        REQUIRE (*(p + 2) == std::byte(0xBB));
        REQUIRE (*(p + 3) == std::byte(0xAA));
      }
    }
  } // end given
  GIVEN ("Casting the address of a 4 byte signed int to a non-const pointer") {
    std::int32_t x = 0xDEADBEEF;

    WHEN ("casting std::int32_t* to non-const std::byte*") {
      std::byte* p = chops::cast_ptr_to<std::byte> (&x);
      THEN ("the bytes should be 0xEF, 0xBE, 0xAD, 0xDE") {
        REQUIRE (*(p + 0) == std::byte(0xEF));
        REQUIRE (*(p + 1) == std::byte(0xBE));
        REQUIRE (*(p + 2) == std::byte(0xAD));
        REQUIRE (*(p + 3) == std::byte(0xDE));
      }
    }
  } // end given
  GIVEN ("Casting the pointer type to std::byte and back for the same type") {
    std::int32_t x = 0xDEADBEEF;

    WHEN ("casting std::int32_t* to const std::byte* and back") {
      const std::byte* p = chops::cast_ptr_to<std::byte> (&x);
      std::int32_t val;
      val = *(chops::cast_ptr_to<std::int32_t>(p));
      THEN ("the two values compare equal") {
        REQUIRE (val == x);
      }
    }
  } // end given
}
