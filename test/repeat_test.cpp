/** @file
 *
 * @brief Test scenarios for Vittorio Romeo's repeat utility function.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2017-2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch_test_macros.hpp"

#include "utility/repeat.hpp"

int gSum = 0;

void myfunc_a () {
  gSum += 1;
}

void myfunc_b ( int i ) {
  REQUIRE (gSum == i);
  gSum += 1;
}

TEST_CASE ( "Vittorio Romeo's repeat utility is a function template to repeat code N times", "[repeat]" ) {
  constexpr int N = 50;
  SECTION ( "A global counter set to 0 and an iteration count set to N" ) {
    gSum = 0;
    chops::repeat(N, myfunc_a);
    REQUIRE (gSum == N);
  }
  SECTION ( "A function that does care about the passed in index is invoked" ) {
    gSum = 0;
    chops::repeat(N, myfunc_b);
    REQUIRE (gSum == N);
  }
  SECTION ( "A lambda func that doesn't care about the passed in index is invoked ") {
    gSum = 0;
    chops::repeat(N, [] { myfunc_a(); } );
    REQUIRE (gSum == N);
  }
  SECTION ( "A lambda func that does care about the passed in index is invoked" ) {
    gSum = 0;
    chops::repeat(N, [] (int i) { myfunc_b(i); } );
    REQUIRE (gSum == N);
  }
  SECTION ( "A lambda func that doesn't care about the index but has a local var is invoked" ) {
    int lSum = 0;
    chops::repeat(N, [&lSum] { lSum += 1; } );
    REQUIRE (lSum == N);
  }
  SECTION ( "A lambda func that does care about the index and has a local var is invoked ") {
    int lSum = 0;
    chops::repeat(N, [&lSum] (int i) { REQUIRE (lSum == i); lSum += 1; } );
    REQUIRE (lSum == N);
  }

}
