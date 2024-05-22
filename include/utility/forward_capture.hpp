/** @file
 *
 * @brief A utility to perfectly forward function template parameters in lambda captures.
 *
 * This code is copied from Vittorio Romeo's blog at https://vittorioromeo.info/, 
 * specifically: 
 * https://vittorioromeo.info/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html.
 *
 * It is not easy to perfectly forward function template parameters in a lambda capture. In
 * particular, a function object passed through a forwarding reference should be passed as either 
 * an rvalue or lvalue reference, depending on what was passed in (which of course is what a
 * forwarding reference is all about). Capturing and preserving the forwarding reference in a lambda
 * is complicated (more so than it should be). This utility provides a wrapper class, which (as in 
 * so many software design problems) solves the problem with a layer of indirection.
 * 
 * @authors Vittorio Romeo, Cliff Green
 *
 * @copyright (c) 2019-2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef FORWARD_CAPTURE_HPP_INCLUDED
#define FORWARD_CAPTURE_HPP_INCLUDED

#include <tuple>
#include <utility> // std::forward

#define CHOPS_FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

namespace chops {

namespace detail {

template <typename... Ts>
auto fwd_capture(Ts&&... xs) {
    return std::tuple<Ts...>(CHOPS_FWD(xs)...); 
}

}

template <typename T>
decltype(auto) access(T&& x) { return std::get<0>(CHOPS_FWD(x)); }

} // end namespace

#define CHOPS_FWD_CAPTURE(...) chops::detail::fwd_capture(CHOPS_FWD(__VA_ARGS__))

#endif

