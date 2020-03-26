/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for @c mutable_shared_buffer and
 *  @c const_shared_buffer classes.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2017-2018 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch.hpp"


#include <cstddef> // std::byte
#include <list>
#include <string_view>

#include "marshall/shared_buffer.hpp"

#include "utility/repeat.hpp"
#include "utility/make_byte_array.hpp"
#include "utility/cast_ptr_to.hpp"

constexpr std::byte Harhar { 42 };
constexpr int N = 11;


template <typename SB, typename PT>
void generic_pointer_construction_test() {
  auto arr = chops::make_byte_array( 40, 41, 42, 43, 44, 60, 59, 58, 57, 56, 42, 42 );
  const PT* ptr = chops::cast_ptr_to<PT>(arr.data());
  SB sb(ptr, arr.size());
  REQUIRE_FALSE (sb.empty());
  const std::byte* buf = chops::cast_ptr_to<std::byte>(arr.data());
  chops::repeat(static_cast<int>(arr.size()), [&sb, arr] (int i) { REQUIRE(*(sb.data()+i) == arr[i]); } );
}

template <typename SB>
void common_methods_test(const std::byte* buf, typename SB::size_type sz) {

  REQUIRE (sz > 2);

  SB sb(buf, sz);
  REQUIRE_FALSE (sb.empty());
  {
    SB sb2(buf, sz);
    REQUIRE_FALSE (sb2.empty());
    REQUIRE (sb == sb2);
  }
  {
    std::list<std::byte> lst (buf, buf+sz);
    SB sb2(lst.cbegin(), lst.cend());
    REQUIRE_FALSE (sb2.empty());
    REQUIRE (sb == sb2);
  }
  {
    auto ba = chops::make_byte_array(buf[0], buf[1]);
    SB sb2(ba.cbegin(), ba.cend());
    REQUIRE_FALSE (sb2.empty());
    REQUIRE (sb2 < sb);
    REQUIRE (sb2 != sb);
  }
  {
    auto ba = chops::make_byte_array(0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    SB sb2(ba.cbegin(), ba.cend());
    REQUIRE_FALSE (sb2.empty());
    REQUIRE (sb2 != sb);
  }
}

template <typename SB>
void byte_vector_move_test() {

  auto arr = chops::make_byte_array (0x01, 0x02, 0x03, 0x04, 0x05);

  std::vector<std::byte> bv { arr.cbegin(), arr.cend() };
  SB sb(std::move(bv));
  REQUIRE (sb == SB(arr.cbegin(), arr.cend()));
  REQUIRE_FALSE (bv.size() == sb.size());
}
 
TEMPLATE_TEST_CASE ( "Checking generic pointer construction",
                     "[common]",
                     char, unsigned char, signed char ) {
  generic_pointer_construction_test<chops::mutable_shared_buffer, TestType>();
  generic_pointer_construction_test<chops::const_shared_buffer, TestType>();
}

                     
TEMPLATE_TEST_CASE ( "Shared buffer common methods test",
                     "[const_shared_buffer] [common]",
                     chops::mutable_shared_buffer, chops::const_shared_buffer ) {
  auto arr = chops::make_byte_array ( 80, 81, 82, 83, 84, 90, 91, 92 );
  common_methods_test<TestType>(arr.data(), arr.size());
}

SCENARIO ( "Mutable shared buffer copy construction and assignment",
           "[mutable_shared_buffer] [copy]" ) {

  GIVEN ("A default constructed mutable shared_buffer") {

    auto arr = chops::make_byte_array ( 80, 81, 82, 83, 84, 90, 91, 92 );

    chops::mutable_shared_buffer sb;
    REQUIRE (sb.empty());

    WHEN ("Another mutable shared buffer is assigned into it") {
      chops::mutable_shared_buffer sb2(arr.cbegin(), arr.cend());
      sb = sb2;
      THEN ("the size has changed and the two shared buffers compare equal") {
        REQUIRE (sb.size() == arr.size());
        REQUIRE (sb == sb2);
      }
    }
    AND_WHEN ("Another mutable shared buffer is copy constructed") {
      sb = chops::mutable_shared_buffer(arr.cbegin(), arr.cend());
      chops::mutable_shared_buffer sb2(sb);
      THEN ("the two shared buffers compare equal and a change to the first shows in the second") {
        REQUIRE (sb == sb2);
        *(sb.data()+0) = Harhar;
        *(sb.data()+1) = Harhar;
        REQUIRE (sb == sb2);
      }
    }
  } // end given
}

SCENARIO ( "Mutable shared buffer resize and clear",
           "[mutable_shared_buffer] [resize_and_clear]" ) {

  GIVEN ("A default constructed mutable shared_buffer") {
    chops::mutable_shared_buffer sb;
    WHEN ("Resize is called") {
      sb.resize(N);
      THEN ("the internal buffer will have all zeros") {
        REQUIRE (sb.size() == N);
        chops::repeat(N, [&sb] (const int& i) { REQUIRE (*(sb.data() + i) == std::byte{0} ); } );
      }
    }
    AND_WHEN ("Another mutable shared buffer with a size is constructed") {
      sb.resize(N);
      chops::mutable_shared_buffer sb2(N);
      THEN ("the two shared buffers compare equal, with all zeros in the buffer") {
        REQUIRE (sb == sb2);
        chops::repeat(N, [&sb, &sb2] (const int& i) {
          REQUIRE (*(sb.data() + i) == std::byte{0} );
          REQUIRE (*(sb2.data() + i) == std::byte{0} );
        } );
      }
    }
    AND_WHEN ("The mutable shared buffer is cleared") {
      sb.resize(N);
      sb.clear();
      THEN ("the size will be zero and the buffer is empty") {
        REQUIRE (sb.size() == 0);
        REQUIRE (sb.empty());
      }
    }
  } // end given
}

SCENARIO ( "Mutable shared buffer swap",
           "[mutable_shared_buffer] [swap]" ) {

  GIVEN ("Two mutable shared_buffers") {
    auto arr1 = chops::make_byte_array (0xaa, 0xbb, 0xcc);
    auto arr2 = chops::make_byte_array (0x01, 0x02, 0x03, 0x04, 0x05);

    chops::mutable_shared_buffer sb1(arr1.cbegin(), arr1.cend());
    chops::mutable_shared_buffer sb2(arr2.cbegin(), arr2.cend());

    WHEN ("The buffers are swapped") {
      chops::swap(sb1, sb2);
      THEN ("the sizes and contents will be swapped") {
        REQUIRE (sb1.size() == arr2.size());
        REQUIRE (sb2.size() == arr1.size());
        REQUIRE (*(sb1.data()+0) == *(arr2.data()+0));
        REQUIRE (*(sb1.data()+1) == *(arr2.data()+1));
        REQUIRE (*(sb2.data()+0) == *(arr1.data()+0));
        REQUIRE (*(sb2.data()+1) == *(arr1.data()+1));
      }
    }
  } // end given
}

SCENARIO ( "Mutable shared buffer append",
           "[mutable_shared_buffer] [append]" ) {

  auto arr = chops::make_byte_array (0xaa, 0xbb, 0xcc);
  auto arr2 = chops::make_byte_array (0xaa, 0xbb, 0xcc, 0xaa, 0xbb, 0xcc);
  chops::mutable_shared_buffer ta(arr.cbegin(), arr.cend());
  chops::mutable_shared_buffer ta2(arr2.cbegin(), arr2.cend());

  GIVEN ("A default constructed mutable shared_buffer") {
    chops::mutable_shared_buffer sb;
    WHEN ("Append with a pointer and size is called") {
      sb.append(arr.data(), arr.size());
      THEN ("the internal buffer will contain the appended data") {
        REQUIRE (sb == ta);
      }
    }
    AND_WHEN ("Append with a mutable shared buffer is called") {
      sb.append(ta);
      THEN ("the internal buffer will contain the appended data") {
        REQUIRE (sb == ta);
      }
    }
    AND_WHEN ("Append is called twice") {
      sb.append(ta);
      sb.append(ta);
      THEN ("the internal buffer will contain twice the appended data") {
        REQUIRE (sb == ta2);
      }
    }
    AND_WHEN ("Appending with single bytes") {
      sb.append(std::byte(0xaa));
      sb.append(std::byte(0xbb));
      sb += std::byte(0xcc);
      THEN ("the internal buffer will contain the appended data") {
        REQUIRE (sb == ta);
      }
    }
    AND_WHEN ("Appending with a char* to test templated append") {
      std::string_view sv("Haha, Bro!");
      chops::mutable_shared_buffer cb(sv.data(), sv.size());
      sb.append(sv.data(), sv.size());
      THEN ("the internal buffer will contain the appended data") {
        REQUIRE (sb == cb);
      }
    }
  } // end given
}

SCENARIO ( "Compare a mutable shared_buffer with a const shared buffer",
           "[mutable_shared_buffer] [const_shared_buffer] [compare]" ) {

  GIVEN ("An array of bytes") {
    auto arr = chops::make_byte_array (0xaa, 0xbb, 0xcc);
    WHEN ("A mutable_shared_buffer and a const_shared_buffer are created from the bytes") {
      chops::mutable_shared_buffer msb(arr.cbegin(), arr.cend());
      chops::const_shared_buffer csb(arr.cbegin(), arr.cend());
      THEN ("the shared buffers will compare equal") {
        REQUIRE (msb == csb);
        REQUIRE (csb == msb);
      }
    }
  } // end given
}

SCENARIO ( "Mutable shared buffer move into const shared buffer",
           "[mutable_shared_buffer] [const_shared_buffer] [move]" ) {

  auto arr1 = chops::make_byte_array (0xaa, 0xbb, 0xcc);
  auto arr2 = chops::make_byte_array (0x01, 0x02, 0x03, 0x04, 0x05);

  GIVEN ("A mutable_shared_buffer") {
    chops::mutable_shared_buffer msb(arr1.cbegin(), arr1.cend());
    WHEN ("A const_shared_buffer is move constructed from the mutable_shared_buffer") {
      chops::const_shared_buffer csb(std::move(msb));
      THEN ("the const_shared_buffer will contain the data and the mutable_shared_buffer will not") {
        REQUIRE (csb == chops::const_shared_buffer(arr1.cbegin(), arr1.cend()));
        REQUIRE_FALSE (msb == csb);
        msb.clear();
        msb.resize(arr2.size());
        msb.append(arr2.data(), arr2.size());
        REQUIRE_FALSE (msb == csb);
      }
    }
  } // end given
}

TEMPLATE_TEST_CASE ( "Move a vector of bytes into a shared buffer",
                     "[common] [move_byte_vec]",
                     chops::mutable_shared_buffer, chops::const_shared_buffer ) {

  byte_vector_move_test<TestType>();

}

SCENARIO ( "Use get_byte_vec for external modification of buffer",
           "[mutable_shared_buffer] [get_byte_vec]" ) {

  auto arr = chops::make_byte_array (0xaa, 0xbb, 0xcc);
  chops::mutable_shared_buffer::byte_vec bv (arr.cbegin(), arr.cend());

  GIVEN ("A mutable_shared_buffer") {
    chops::mutable_shared_buffer msb(bv.cbegin(), bv.cend());

    WHEN ("get_byte_vec is called") {
      auto r = msb.get_byte_vec();
      THEN ("the refererence can be used to access and modify data") {
        REQUIRE (r == bv);
        r[0] = std::byte(0xdd);
        REQUIRE_FALSE (r == bv);
      }
    }
  } // end given

}

