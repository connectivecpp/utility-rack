/** @file
 * 
 * @brief Overloaded utility function and class, used with @c std::visit.
 *
 * This utility simplifies calling visitor function objects on a `std::variant`
 * based on the instantiated variant type. The code is explicitly copied from 
 * the _cppreference_ website.
 * 
 * See https://en.cppreference.com/w/cpp/utility/variant/visit2 for the C++ 17
 * and C++ 20 version of `std::visit`.
 *
 * The `overloaded` function template is not needed in C++ 20.
 *
 * C++ 26 has a `visit` method as part of the `std::variant` class that is more 
 * flexible, see https://en.cppreference.com/w/cpp/utility/variant/visit for 
 * additional info.
 *  
 * @author Cliff Green
 * 
 * @copyright (c) 2019-2025 by Cliff Green
 * 
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef OVERLOADED_HPP_INCLUDED
#define OVERLOADED_HPP_INCLUDED

namespace chops {
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // end namespace

#endif

