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

constexpr int base = 3;

struct copyable_foo {
  copyable_foo() = default;
  copyable_foo(const copyable_foo& rhs) {
    ++copy_count;
    val = rhs.val;
  }
  copyable_foo(copyable_foo&& rhs) {
    ++move_count;
    val = rhs.val;
  }
  copyable_foo& operator= (const copyable_foo&) = delete;
  copyable_foo& operator= (copyable_foo&&) = delete;

  inline static int copy_count = 0;
  inline static int move_count = 0;
  int val{base};

  int operator()(int i) { val += i; return val; }
};

struct movable_foo {
  movable_foo() = default;
  movable_foo(const movable_foo&) = delete;
  movable_foo(movable_foo&& rhs) {
    ++move_count;
    val = std::move(rhs.val);
  }
  movable_foo& operator= (const movable_foo&) = delete;
  movable_foo& operator= (movable_foo&&) = delete;

  inline static int copy_count = 0;
  inline static int move_count = 0;
  std::unique_ptr<int> val{std::make_unique<int>(base)};

  int operator()(int i) { *val += i; return *val; }
};


template <typename F>
auto test_func(F&& f_obj, int val1, int val2) {
  return [func = CHOPS_FWD_CAPTURE(f_obj), val1, val2] () mutable {
    return chops::access(func)(val1) + chops::access(func)(val2);
  };
}


TEST_CASE( "Vittorio Romeo's perfect forward parameters for lambda capture utility",
          "[forward_capture]" ) {

  // lvalue reference captured, will modify original object
  {
    copyable_foo a{};
    auto lam = test_func(a, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    REQUIRE (a.val == 6);
    REQUIRE (a.copy_count == 0);
    REQUIRE (a.move_count == 0);
    i = lam();
    REQUIRE (i == 16);
    REQUIRE (a.val == 9);
    REQUIRE (a.copy_count == 0);
    REQUIRE (a.move_count == 0);
    i = lam();
    REQUIRE (i == 22);
    REQUIRE (a.val == 12);
    REQUIRE (a.copy_count == 0);
    REQUIRE (a.move_count == 0);
  }
  // rvalue, copy made and stored in lambda
  {
    auto lam = test_func(copyable_foo{}, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    i = lam();
    REQUIRE (i == 16);
    i = lam();
    REQUIRE (i == 22);
  }
  // rvalue move captured
  {
    movable_foo a{};
    auto lam = test_func(a, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    i = lam();
    REQUIRE (i == 16);
    i = lam();
    REQUIRE (i == 22);
  }
  // rvalue move captured
  {
    auto lam = test_func(movable_foo{}, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    i = lam();
    REQUIRE (i == 16);
    i = lam();
    REQUIRE (i == 22);
  }
}

