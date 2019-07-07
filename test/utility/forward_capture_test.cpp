/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for Vittorio Romeo's perfect forward capture utilities.
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

#include <memory> // std::unique_ptr

#include "utility/forward_capture.hpp"

struct copyable_foo {
  int val{42};
};

struct movable_foo {
  std::unique_ptr<int> val{std::make_unique(43)};
};


template <typename F>
void func(F&&

auto foo = [](auto&& a)
{
    return [a = CHOPS_FWD_CAPTURE(a)]() mutable 
    { 
        ++access(a)._value;
        std::cout << access(a)._value << "\n";
    };
};

int main()
{
    {
        auto l_inner = foo(A{});
        l_inner(); // Prints `1`.
        l_inner(); // Prints `2`.
        l_inner(); // Prints `3`.
    }
    
    {
        A my_a;
        auto l_inner = foo(my_a);
        l_inner(); // Prints `1`.
        l_inner(); // Prints `2`.
        l_inner(); // Prints `3`.

        // Prints '3', yay!
        std::cout << my_a._value << "\n";
    }
}

int gSum = 0;

void myfunc_a () {
  gSum += 1;
}

void myfunc_b ( int i ) {
  REQUIRE (gSum == i);
  gSum += 1;
}

SCENARIO( "Vittorio Romeo's perfect forward parameters for lambda capture utility",
          "[forward_capture]" ) {
  constexpr int N = 50;
  GIVEN ("A global counter set to 0 and an iteration count set to N") {
    WHEN ("A function that doesn't care about the passed in index is invoked") {
      gSum = 0;
      chops::repeat(N, myfunc_a);
      THEN ("the global counter should now equal N") {
        REQUIRE (gSum == N);
      }
    }
    AND_WHEN ("A function that does care about the passed in index is invoked") {
      gSum = 0;
      chops::repeat(N, myfunc_b);
      THEN ("the global counter should now equal N") {
        REQUIRE (gSum == N);
      }
    }
    AND_WHEN ("A lambda func that doesn't care about the passed in index is invoked") {
      gSum = 0;
      chops::repeat(N, [] { myfunc_a(); } );
      THEN ("the global counter should now equal N") {
        REQUIRE (gSum == N);
      }
    }
    AND_WHEN ("A lambda func that does care about the passed in index is invoked") {
      gSum = 0;
      chops::repeat(N, [] (int i) { myfunc_b(i); } );
      THEN ("the global counter should now equal N") {
        REQUIRE (gSum == N);
      }
    }
    AND_WHEN ("A lambda func that doesn't care about the index but has a local var is invoked") {
      int lSum = 0;
      chops::repeat(N, [&lSum] { lSum += 1; } );
      THEN ("the local counter should now equal N") {
        REQUIRE (lSum == N);
      }
    }
    AND_WHEN ("A lambda func that does care about the index and has a local var is invoked") {
      int lSum = 0;
      chops::repeat(N, [&lSum] (int i) { REQUIRE (lSum == i); lSum += 1; } );
      THEN ("the local counter should now equal N") {
        REQUIRE (lSum == N);
      }
    }
  } // end given
}
