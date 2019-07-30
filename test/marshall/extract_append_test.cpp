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

SCENARIO ( "Extract variable length integers",
          "[extract_var_int]" ) {
    
    GIVEN ("A standard byte") {
        WHEN ("extract_var_int is called with a byte of less than 127") {
            //Decodes an unsigned variable-length integer using the MSB algorithm.
            std::byte test_buf [7];
//            test_buf[0] = static_cast<std::byte>(0xCA);
//            test_buf[1] = static_cast<std::byte>(0xFE);
            test_buf[0] = static_cast<std::byte>(0xCA);
            test_buf[1] = static_cast<std::byte>(0xFE);
            // the u in 2u means unsigned integer
            auto val1 = chops::extract_var_int<std::uint8_t>(test_buf, 3u);
            THEN("the extracted variable should be correct"){
                REQUIRE(val1 == 126);
            }
        }
        
    }
    
}

TEST_CASE ( "Append variable length integers",
          "[append_var_int]" ) {
//
//    GIVEN ("A standard byte") {
//        WHEN ("append_var_int is called with a byte of more than 127") {
            std::byte test_buf [7];
            auto outsize = chops::append_var_int<int>(test_buf, 0xCAFE);
    REQUIRE(static_cast<int> (test_buf[0]) == 254);
    INFO("test buf [0]" << static_cast<int> (test_buf[0]))
    REQUIRE(static_cast<int> (test_buf[1]) == 149);
    INFO("test buf [1]" << static_cast<int> (test_buf[1]))
    REQUIRE(static_cast<int> (test_buf[2]) == 3);
    INFO("test buf [2]" << static_cast<int> (test_buf[2]))
            // Encodes an unsigned variable-length integer using the MSB algorithm
            //Should print 51966

                REQUIRE(outsize == 3);
            auto output = chops::extract_var_int<int>(test_buf, outsize);
    
                REQUIRE(output == 51966);

                REQUIRE(outsize == 3);

        
    }
    



TEST_CASE( "Extracting variable intgers", "[extract_var_int]" ) {
    //before 127 after 128 after 4 bytes
    // encode and decode
    // static_cast<std::byte>(0x04))
    std::byte test_buf [7];
//    std::byte my_byte = static_cast<std::byte>(0xCAFE);
//    std::byte my_byte{ 0xCAFE };
    test_buf[0] = static_cast<std::byte>(0xCA);
    test_buf[0] = static_cast<std::byte>(0xFE);
//    uint8_t* buf;
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 2u);
    
    // VVVVVVV this isn't going to work below VVVVVVVVV
    REQUIRE( val1 == 126 );
}
TEST_CASE(){}

//0XCAFE 126
//[ca] [fe]
//
//0XCaff 127
//[ca] [ff]
//0xcb00 128

