/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for @c extract_val and @c append_val functions.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2019 by Cliff Green, Roxanne Agerone
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

constexpr std::uint32_t val1 = 0xDDCCBBAA;
constexpr char val2 = 0xEE;
constexpr std::int16_t val3 = 0x01FF;
constexpr std::uint64_t val4 = 0x0908070605040302;
constexpr std::int32_t val5 = 0xDEADBEEF;

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
// 0, 1, 2, 127, 128, transform back an forth bw functions
SCENARIO ( "Append values into a buffer",
           "[append_val]" ) {

  GIVEN ("An empty byte buffer") {
    std::byte buf[arr_sz];

    WHEN ("Append_val is called with a single value") {
      constexpr std::uint32_t v = 0x04030201;
      REQUIRE(chops::append_val(buf, v) == 4u);
      THEN ("the buffer will contain the single value in network endian order") {
        REQUIRE (buf[0] == static_cast<std::byte>(0x04));
        REQUIRE (buf[1] == static_cast<std::byte>(0x03));
        REQUIRE (buf[2] == static_cast<std::byte>(0x02));
        REQUIRE (buf[3] == static_cast<std::byte>(0x01));
      }
    }
    AND_WHEN ("Append_val is called with multiple values") {
      std::byte* ptr = buf;
      REQUIRE(chops::append_val(ptr, val1) == 4u); ptr += sizeof(val1);
      REQUIRE(chops::append_val(ptr, val2) == 1u); ptr += sizeof(val2);
      REQUIRE(chops::append_val(ptr, val3) == 2u); ptr += sizeof(val3);
      REQUIRE(chops::append_val(ptr, val4) == 8u); ptr += sizeof(val4);
      REQUIRE(chops::append_val(ptr, val5) == 4u);
      
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


TEST_CASE ( "Append and extract variable length integers","[append_var_int]" ) {

    std::byte test_buf [7];
    auto outsize = chops::append_var_int<int>(test_buf, 0xCAFE);
    REQUIRE(static_cast<int> (test_buf[0]) == 254);
    INFO("test buf [0]" << static_cast<int> (test_buf[0]))
    REQUIRE(static_cast<int> (test_buf[1]) == 149);
    INFO("test buf [1]" << static_cast<int> (test_buf[1]))
    REQUIRE(static_cast<int> (test_buf[2]) == 3);
    INFO("test buf [2]" << static_cast<int> (test_buf[2]))

    auto output = chops::extract_var_int<int>(test_buf, outsize);
    
    REQUIRE(output == 51966);
    REQUIRE(outsize == 3);
    }

TEST_CASE ( "Append var len integer of 127","[append_var_int]" ) {
    
    std::byte test_buf [7];
    auto outsize = chops::append_var_int<int>(test_buf, 0x7F);
    REQUIRE(static_cast<int> (test_buf[0]) == 127);
    REQUIRE(outsize == 1);
}
TEST_CASE ( "Append var len integer of 128","[append_var_int]" ) {
    
    std::byte test_buf [7];
    auto outsize = chops::append_var_int<int>(test_buf, 0x80);
    REQUIRE(static_cast<int> (test_buf[0]) == 128); //byte flag set
    REQUIRE(static_cast<int> (test_buf[1]) == 1);
    REQUIRE(outsize == 2);
}
TEST_CASE ( "Append var len integer larger than 4 bytes","[append_var_int]" ) {
    
    std::byte test_buf [7];
    auto outsize = chops::append_var_int<int>(test_buf, 0x10000000);
    REQUIRE(static_cast<int> (test_buf[0]) == 128); //byte flag set
    REQUIRE(static_cast<int> (test_buf[4]) == 1);
    REQUIRE(outsize == 5);
}

TEST_CASE( "Extracting variable integer larger than two bytes", "[extract_var_int]" ) {
    std::byte test_buf [7];
    test_buf[0] = static_cast<std::byte>(0xFE); // encodes to 126 with byte flag set
    test_buf[1] = static_cast<std::byte>(0xCA); // encodes to 9472
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 2u);
    // 126 + 9472 = 9598
    REQUIRE( val1 == 9598 );
}

TEST_CASE("Extracting variable integer smaller than a byte", "[extract_var_int]"){
    std::byte test_buf [7];
    test_buf[0] = static_cast<std::byte>(0x7F); // encodes to 127
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 1u);
    REQUIRE( val1 == 127 );
}

TEST_CASE("Extracting variable integer of 128", "[extract_var_int]"){
    std::byte test_buf [7];
    test_buf[0] = static_cast<std::byte>(0x80); // encodes to 0 with byte flag set
    test_buf[1] = static_cast<std::byte>(0x01); // encodes to 128
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 2u);
    // 127 + 1 = 128
    REQUIRE( val1 == 128 );
}
