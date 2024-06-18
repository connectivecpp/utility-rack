/** @file
 *
 * @brief Test scenarios for @c overloaded utility class and function.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2019-2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <variant>
#include <optional>

#include "catch2/catch_test_macros.hpp"
#include "utility/overloaded.hpp"

using var_t = std::variant<double, int, std::string, std::optional<float>>;

constexpr int double_val = 43;
constexpr int int_val = 44;
constexpr int string_val = 45;
constexpr int optional_val_present = 46;
constexpr int optional_val_not_present = 47;

int query_variant (const var_t& v) {
  int ret_val = 0;
  std::visit(chops::overloaded {
      [&ret_val] (double val) { ret_val = double_val; },
      [&ret_val] (int val) { ret_val = int_val; },
      [&ret_val] (std::string val) { ret_val = string_val; },
      [&ret_val] (std::optional<float> val) { ret_val = val ? 
                                           optional_val_present : optional_val_not_present; },
    }, v);
  return ret_val;
}


TEST_CASE ( "Creating an overloaded function object using template parameter packs", "[overloaded]" ) {

  var_t my_var;

  SECTION ( "setting the variant to a double type" ) {
    my_var = 11.0;
    REQUIRE (query_variant(my_var) == double_val);
  }
  SECTION ( "setting the variant to an int type" ) {
    my_var = 12;
    REQUIRE (query_variant(my_var) == int_val);
  }
  SECTION ( "setting the variant to a string type" ) {
    my_var = std::string("Howdy!");
    REQUIRE (query_variant(my_var) == string_val);
  }
  SECTION ( "setting the variant to an optional float type with a val" ) {
    my_var = std::optional<float>(66.0f);
    REQUIRE (query_variant(my_var) == optional_val_present);
  }
  SECTION ( "setting the variant to an optional float type without a val " ) {
    my_var = std::optional<float>();
    REQUIRE (query_variant(my_var) == optional_val_not_present);
  }

}
