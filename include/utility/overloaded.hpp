/** @file
 * 
 *  @ingroup utility_module
 * 
 *  @brief Overloaded utility function and class, primarily used with @c std::visit.
 * 
 *  See https://en.cppreference.com/w/cpp/utility/variant/visit for explanations
 *  and usage. This is the basic and simple version of the code as copied and pasted
 *  from the cppreference site. There is likely to be a more elaborate @c std::overload 
 *  in a future C++ standard (probably post-C++20 - see 
 *  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0051r3.pdf).
 *  
 *  @author Cliff Green
 * 
 *  Copyright (c) 2019 by Cliff Green
 * 
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef OVERLOADED_HPP_INCLUDED
#define OVERLOADED_HPP_INCLUDED

namespace chops {
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // end namespace

#endif

