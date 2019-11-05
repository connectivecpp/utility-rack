/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for the @c marshall and @c unmarshall function
 *  templates and related classes and functions.
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

namespace hiking {

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

} // end namespace hiking

namespace chops {

template <typename Buf>
Buf& marshall(Buf& buf, const loc& loc) {
  marshall<std::int32_t>(buf, loc.latitude);
  marshall<std::int32_t>(buf, loc.longitude);
  marshall<std::int16_t>(buf, loc.altitude);
  return buf;
}

template <typename Buf>
Buf& marshall(Buf& buf, const hiking::trail_stats& ts) {
  marshall<std::uint64_t>(buf, ts.length);
  marshall<std::uint16_t>(buf, ts.elev);
  marshall<std::uint8_t, std::uint16_t>(buf, ts.rating);
  return buf;
}

template <typename Buf>
Buf& marshall(Buf& buf, const hiking::hiking_trail& ht) {
  marshall<std::uint16_t>(buf, ht.name);
  marshall<std::uint8_t>(buf, ht.federal);
  marshall(buf, ht.trail_head);
  marshall<std::uint16_t, loc>(buf, ht.intersections.size(), ht.intersections.cbegin());
  marshall(buf, ht.stats);
  return buf;
}

} // end namespace chops

template <typename Buf>
void test_marshall () {

  Buf buf;

  chops::marshall<std::uint16_t>(buf, 42);

  const loc pt1 { 42, 43, 21 };
  const loc pt2 { 62, 63, 11 };

  chops::marshall(buf, pt1);
  chops::marshall(buf, pt2);

  const hiking::trail_stats ts1 { 101, 51, std::make_optional(201) };
  const hiking::trail_stats ts2 { 301, 41, std::make_optional(401) };

  chops::marshall(buf, ts1);
  chops::marshall(buf, ts2);

  const loc inter1 { 1001, 1002, 500 };
  const loc inter2 { 1003, 1004, 501 };
  const loc inter3 { 1005, 1006, 502 };
  const loc inter4 { 1007, 1008, 503 };
  const loc inter5 { 1009, 1010, 505 };
  const loc inter6 { 1011, 1012, 505 };


  const hiking::hiking_trail hk1 { "Huge trail", true, pt1, { inter1, inter2, inter3 }, ts1 };
  const hiking::hiking_trail hk2 { "Small trail", false, pt2, { inter3, inter4, inter5, inter6 }, ts2 };

  chops::marshall(buf, hk1);
  chops::marshall(buf, hk2);

}

TEMPLATE_TEST_CASE ( "Marshall ", "[marshall] [shared_buffer]",
                     chops::mutable_shared_buffer, std::vector<std::byte>,
                     chops::fixed_size_byte_array<1000> ) {

  test_marshall<TestType>();

}

