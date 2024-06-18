/** @file
 *
 * @brief Test scenarios for @c erase_where utility functions.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2018-2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch_test_macros.hpp"

#include <vector>

#include "utility/erase_where.hpp"

TEST_CASE ( "Richard Hodge's erase_where combines erase with remove", "[erase_where]" ) {

  std::vector<int> vec { 0, 1, 2, 3, 4, 5, 6, 7 };

  SECTION ( "erase_where is called with a value" ) {
    chops::erase_where(vec, 5);
    REQUIRE (vec.size() == 7);
    REQUIRE (vec == (std::vector<int> { 0, 1, 2, 3, 4, 6, 7 }) );
  }
  SECTION ( "erase_where is called with a predicate removing anything < 3" ) {
    chops::erase_where_if(vec, [] (int i) { return i < 3; } );
    REQUIRE (vec.size() == 5);
    REQUIRE (vec == (std::vector<int> { 3, 4, 5, 6, 7 }) );
  }
}
