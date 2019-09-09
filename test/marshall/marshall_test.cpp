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
#include <vector>
#include <array>
#include <optional>

#include "marshall/marshall.hpp"
#include "marshall/shared_buffer.hpp"

struct loc {
  int    latitude;
  int    longitude;
  short  altitude;
};

struct trail_stats {
  long                length;
  short               elev;
  std::optional<int>  rating;
};

struct hiking_trail {
  std::string     name;
  bool            federal;
  loc             trail_head;
  std::list<loc>  intersections;
  trail_stats     stats;
};

namespace mar_test {

using vector_bytes = std::vector<std::byte>;

template <typename Buf>
Buf& marshall(Buf& buf, const loc& loc) {
  chops::marshall<std::int32_t>(buf, loc.latitude);
  chops::marshall<std::int32_t>(buf, loc.longitude);
  return chops::marshall<std::int16_t>(buf, loc.altitude);
}

template <typename Buf>
Buf& marshall(Buf& buf, const trail_stats& ts) {
  chops::marshall<std::uint64_t>(buf, ts.length);
  chops::marshall<std::uint16_t>(buf, ts.elev);
  return chops::marshall<std::uint8_t, std::uint16_t>(buf, ts.rating);
}

template <typename Buf>
Buf& marshall(Buf& buf, const hiking_trail& ht) {
  chops::marshall<std::uint16_t>(buf, ht.name);
  chops::marshall<std::uint8_t>(buf, ht.federal);
  mar_test::marshall(buf, ht.trail_head);
  chops::marshall_sequence<std::uint16_t, loc>(buf, ht.intersections.size(), ht.intersections.cbegin());
  return mar_test::marshall(buf, ht.stats);
}

} // end namespace mar_test

template <typename Buf>
void test_marshall (Buf& buf) {
  const loc pt1 { 42, 43, 21 };
  const loc pt2 { 62, 63, 11 };

  mar_test::marshall(buf, pt1);
  mar_test::marshall(buf, pt2);

  const trail_stats ts1 { 101, 51, std::make_optional(201) };
  const trail_stats ts2 { 301, 41, std::make_optional(401) };

  mar_test::marshall(buf, ts1);
  mar_test::marshall(buf, ts2);

  const loc inter1 { 1001, 1002, 500 };
  const loc inter2 { 1003, 1004, 501 };
  const loc inter3 { 1005, 1006, 502 };
  const loc inter4 { 1007, 1008, 503 };
  const loc inter5 { 1009, 1010, 505 };
  const loc inter6 { 1011, 1012, 505 };


  const hiking_trail hk1 { "Huge trail", true, pt1, { inter1, inter2, inter3 }, ts1 };
  const hiking_trail hk2 { "Small trail", false, pt2, { inter3, inter4, inter5, inter6 }, ts2 };

  mar_test::marshall(buf, hk1);
  mar_test::marshall(buf, hk2);

}

TEST_CASE ( "Marshall using mutable_shared_buffer",
            "[marshall] [shared_buffer]" ) {

  chops::mutable_shared_buffer buf;
  test_marshall(buf);

}

TEST_CASE ( "Marshall using std vector",
            "[marshall] [std_vector]" ) {

//   std::vector<std::byte> buf;
  mar_test::vector_bytes buf;
  test_marshall(buf);

}

TEST_CASE ( "Marshall using adapter on C array",
            "[marshall] [array]" ) {

  std::byte t[1000];
  chops::buf_adapter buf(t);
  test_marshall(buf);

}

TEST_CASE ( "Marshall using adapter on std array",
            "[marshall] [std_array]" ) {

  std::array<std::byte, 1000> t;
  chops::buf_adapter buf(t.data());
  test_marshall(buf);

}

