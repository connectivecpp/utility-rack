/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for @c extract_val and @c append_val functions.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2019 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch.hpp"


#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t, etc

#include "marshall/extract_append.hpp"
#include "utility/repeat.hpp"
#include "utility/make_byte_array.hpp"
#include "utility/cast_ptr_to.hpp"

std::uint32_t val1 = 0xDDCCBBAA;
char val2 = 0xEE;
std::int16_t val3 = 0x01FF;
std::uint64_t val4 = 0x0908070605040302;
std::int32_t val5 = 0xDEADBEEF;

constexpr int arr_sz = sizeof(val1)+sizeof(val2)+sizeof(val3)+sizeof(val4)+sizeof(val5);

auto net_buf = chops::make_byte_array(0xDD, 0xCC, 0xBB, 0xAA, 0xEE, 0x01, 0xFF,
    0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0xDE, 0xAD, 0xBE, 0xEF);

SCENARIO ( "Endian detection",
           "[endian] [little_endian]" ) {

  GIVEN ("A little-endian system") {

    WHEN ("The detect_big_endian function is called") {
      THEN ("the value is false") {
        REQUIRE (!chops::big_endian);
      }
    }
  } // end given
}

SCENARIO ( "Append values into a buffer",
           "[append_val]" ) {

  GIVEN ("An empty byte buffer") {
    std::byte buf[arr_sz];

    WHEN ("Append_val is called with a single value") {
      std::uint32_t v = 0x04030201;
      chops::append_val(buf, v);
      THEN ("the buffer will contain the single value in network endian order") {
        REQUIRE (buf[0] == static_cast<std::byte>(0x04));
        REQUIRE (buf[1] == static_cast<std::byte>(0x03));
        REQUIRE (buf[2] == static_cast<std::byte>(0x02));
        REQUIRE (buf[3] == static_cast<std::byte>(0x01));
      }
    }
    AND_WHEN ("Append_val is called with multiple values") {
      std::byte* ptr = buf;
      chops::append_val(ptr, val1); ptr += sizeof(val1);
      chops::append_val(ptr, val2); ptr += sizeof(val2);
      chops::append_val(ptr, val3); ptr += sizeof(val3);
      chops::append_val(ptr, val4); ptr += sizeof(val4);
      chops::append_val(ptr, val5);
      
      THEN ("the buffer will have all of the values in network endian order") {
        chops::repeat(arr_sz, [&buf] (int i) { REQUIRE (buf[i] == net_buf[i]); } );
      }
    }
  } // end given
}

SCENARIO ( "Extract values from a buffer",
           "[extract_val]" ) {

  GIVEN ("A buffer of bytes in network endian order") {
    WHEN ("Extract_val is called for multiple values") {

      const std::byte* ptr = net_buf.data();
      std::uint32_t v1 = chops::extract_val<std::uint32_t>(ptr); ptr += sizeof(v1);
      char v2 = chops::extract_val<char>(ptr); ptr += sizeof(v2);
      std::int16_t v3 = chops::extract_val<std::int16_t>(ptr); ptr += sizeof(v3);
      std::uint64_t v4 = chops::extract_val<std::uint64_t>(ptr); ptr += sizeof(v4);
      std::int32_t v5 = chops::extract_val<std::int32_t>(ptr);

      THEN ("the values are all in native order") {
        REQUIRE(v1 == val1);
        REQUIRE(v2 == val2);
        REQUIRE(v3 == val3);
        REQUIRE(v4 == val4);
        REQUIRE(v5 == val5);
      }
    }
  } // end given
}

