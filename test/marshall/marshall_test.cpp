/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for the @c marshaller and @c unmarshaller class
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

/*
namespace chops {

template <typename Buf>
void marshall_udt(marshaller<Buf>& m, const loc& loc) {
  m.marshall<std::int32_t>(loc.latitude);
  m.marshall<std::int32_t>(loc.longitude);
  m.marshall<std::int16_t>(loc.altitude);
}

template <typename Buf>
void marshall_udt(marshaller<Buf>& m, const trail_stats& ts) {
  m.marshall<std::uint64_t>(ts.length);
  m.marshall<std::uint16_t>(ts.elev);
  m.marshall<std::uint8_t, std::uint16_t>(ts.rating);
}

template <typename Buf>
void marshall_udt(marshaller<Buf>& m, const hiking_trail& ht) {
  m.marshall<std::uint16_t>(ht.name);
  m.marshall<std::uint8_t>(ht.federal);
  m.marshall(ht.trail_head);
  m.marshall_sequence<std::uint16_t, loc>(ht.intersections.size(), ht.intersections.cbegin());
  m.marshall(ht.stats);
}

} // end namespace chops
*/

template <typename Buf>
void test_marshall () {

  chops::marshaller<Buf> m;

  m.template marshall<std::uint16_t>(42);

/*
  const loc pt1 { 42, 43, 21 };
  const loc pt2 { 62, 63, 11 };

  m.marshall<loc>(pt1);
  m.marshall<loc>(pt2);

  const trail_stats ts1 { 101, 51, std::make_optional(201) };
  const trail_stats ts2 { 301, 41, std::make_optional(401) };

  m.marshall<trail_stats>(ts1);
  m.marshall<trail_stats>(ts2);

  const loc inter1 { 1001, 1002, 500 };
  const loc inter2 { 1003, 1004, 501 };
  const loc inter3 { 1005, 1006, 502 };
  const loc inter4 { 1007, 1008, 503 };
  const loc inter5 { 1009, 1010, 505 };
  const loc inter6 { 1011, 1012, 505 };


  const hiking_trail hk1 { "Huge trail", true, pt1, { inter1, inter2, inter3 }, ts1 };
  const hiking_trail hk2 { "Small trail", false, pt2, { inter3, inter4, inter5, inter6 }, ts2 };

  m.marshall<hiking_trail>(hk1);
  m.marshall<hiking_trail>(hk2);
*/

}

TEST_CASE ( "Marshall using mutable_shared_buffer",
            "[marshall] [shared_buffer]" ) {

  test_marshall<chops::mutable_shared_buffer>();

}

TEST_CASE ( "Marshall using std vector",
            "[marshall] [std_vector]" ) {

  test_marshall<std::vector<std::byte> >();

}

TEST_CASE ( "Marshall using fixed_size_byte_array",
            "[marshall] [fixed_size_byte_array]" ) {

  test_marshall<chops::fixed_size_byte_array<1000> >();

}

