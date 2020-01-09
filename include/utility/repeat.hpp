/** @file
 *
 *  @ingroup utility_module
 *
 *  @brief Repeat code N times, since the C++ standard range-for does not make it
 *  easy to repeat a loop N times. The lambda or function object can access the iteration
 *  count, if desired.
 *
 *  This code is copied from Vittorio Romeo's blog at https://vittorioromeo.info/, specifically
 *  the first article in the series: 
 *  https://vittorioromeo.info/index/blog/abstraction_design_implementation_repeat.html. This
 *  is the same as his, including the noexcept propagation, except that it has been 
 *  simplified so that the count type is not simplified. Vittorio has written another article
 *  showing a compile-time repeat function.
 *
 *  @authors Vittorio Romeo, Cliff Green
 *
 *  Copyright (c) 2017-2019 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef REPEAT_HPP_INCLUDED
#define REPEAT_HPP_INCLUDED

#include <type_traits>

namespace chops {

template <typename F>
constexpr auto repeat(int n, F&& f)
    noexcept(noexcept(f(n)))
    -> std::enable_if_t<std::is_invocable_v<F&&, int>>
{
    for(int i = 0; i < n; ++i)
    {
        f(i);
    }
}

template <typename F>
constexpr auto repeat(int n, F&& f)
    noexcept(noexcept(f()))
    -> std::enable_if_t<!std::is_invocable_v<F&&, int>>
{
    for(int i = 0; i < n; ++i)
    {
        f();
    }
}

} // end namespace

#endif

