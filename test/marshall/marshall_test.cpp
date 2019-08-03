/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for the various @c marshall and @c unmarshall function
 *  templates.
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
#include <list>

#include "marshall/marshall.hpp"

#include "utility/repeat.hpp"
#include "utility/make_byte_array.hpp"
#include "utility/cast_ptr_to.hpp"

struct location {
  int    latitude;
  int    longitude;
  short  altitude;
};

struct boundary {
  std::string          name;
  bool                 government;
  std::list<location>  points;
};

namespace chops {

template <typename Buf>
Buf& marshall(Buf& buf, const location& loc) {
  return marshall<std::int16_t>(loc.altitude, marshall<std::int32_t>(loc.longitude, 
                                marshall<std::int32_t>(loc.latitude, buf)));
}

/*
template <typename Buf>
Buf& marshall(Buf& buf, const boundary& b) {
  marshall<std::uint16_t>(buf, b.name);
  marshall<std::uint8_t>(buf, b.government);
  marshall_sequence<std::uint16_t
  
}
*/

} // end namespace chops

template <typename Buf>
void test_marshall () {
  Buf buf;
  location pt1 { 42, 43, 21 };
  location pt2 { 62, 63, 11 };

  chops::marshall(buf, pt1);
  chops::marshall(buf, pt2);

}

TEST_CASE ( "Marshall using mutable_shared_buffer",
            "[marshall] [shared_buffer]" ) {

}

TEST_CASE ( "Marshall using std vector",
            "[marshall] [std_vector]" ) {

}

TEST_CASE ( "Marshall using adaptor on C array",
            "[marshall] [array]" ) {

}

TEST_CASE ( "Marshall using adaptor on std array",
            "[marshall] [std_array]" ) {

}
