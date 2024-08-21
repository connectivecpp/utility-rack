/** @file
 *
 * @brief Utility functions to construct and compare a @c std::array of 
 * @c std::byte objects.
 *
 * A @c std::byte is defined as an enum and there are no implicit conversions from int
 * to an enum class. Instead of writing:
 *
 * @code
 * std::array<std::byte, 5> arr = 
 *   { std::byte{0x36}, std::byte{0xd0}, std::byte{0x42}, std::byte{0xbe}, std::byte{0xef} };
 * @endcode
 * it is easier (specially for larger arrays) to write:
 * @code
 * auto arr = chops::make_byte_array(0x36, 0xd0, 0x42, 0xbe, 0xef);
 * @endcode
 *
 * The @c make_byte_array function is taken from an example by Blitz Rakete on Stackoverflow 
 * (Blitz is user Rakete1111 on SO). The @c static_cast was added to eliminate
 * "narrowing" warnings (since typically integer values are used to initialize the
 * @c std::byte values, which would be a narrowing conversion).
 *
 * The @c compare_byte_arrays function simplifies test code, typically using the
 * Catch2 unit test library.
 * 
 * @authors Blitz Rakete, Cliff Green
 *
 * @copyright (c) 2017-2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef MAKE_BYTE_ARRAY_HPP_INCLUDED
#define MAKE_BYTE_ARRAY_HPP_INCLUDED

#include <array>
#include <cstddef> // std::byte
#include <utility> // std::forward

namespace chops {

template<typename... Ts>
constexpr std::array<std::byte, sizeof...(Ts)> make_byte_array(Ts&&... args) noexcept {
  return { std::byte{static_cast<std::byte>(std::forward<Ts>(args))}... };
}

template <typename std::size_t N>
constexpr bool compare_byte_arrays (const std::array<std::byte, N>& lhs, const std::array<std::byte, N>& rhs) {
  return lhs == rhs;
}

} // end namespace

#endif

