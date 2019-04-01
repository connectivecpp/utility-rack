/** @file
 *
 *  @defgroup marshall_module Class and functions for data marshalling (transform
 *  objects into and out of byte streams for transmission over a network or file IO).
 *
 *  @ingroup marshall_module
 *
 *  @brief Byte swapping and endian detection utility functions.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2019 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef BYTE_SWAP_HPP_INCLUDED
#define BYTE_SWAP_HPP_INCLUDED

#include "utility/cast_ptr_to.hpp"

#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint32_t
#include <utility> // std::swap
#include <type_traits> // std::is_integral

namespace chops {

namespace detail {

// since function template partial specialization is not supported (yet) in C++,
// overload the swap function template with a parameter corresponding to the sizeof the
// value being swapped; no data is passed in the second parameter, only using the type 
// for overloading

template <unsigned int>
struct size_tag { };

// if char / byte, no swap needed
template <typename T>
constexpr void byte_swap(T&, const size_tag<1u>*) noexcept {
}

// std::swap is not constexpr until C++ 20, so the following functions won't be
// constexpr until then; however, it doesn't hurt to think ahead
template <typename T>
constexpr void byte_swap(T& val, const size_tag<2u>*) noexcept {
  std::byte* p = cast_ptr_to<std::byte>(&val);
  std::swap (*(p+0), *(p+1));
}

template <typename T>
constexpr void byte_swap(T& val, const size_tag<4u>*) noexcept {
  std::byte* p = cast_ptr_to<std::byte>(&val);
  std::swap (*(p+0), *(p+3));
  std::swap (*(p+1), *(p+2));
}

template <typename T>
constexpr void byte_swap(T& val, const size_tag<8u>*) noexcept {
  std::byte* p = cast_ptr_to<std::byte>(&val);
  std::swap (*(p+0), *(p+7));
  std::swap (*(p+1), *(p+6));
  std::swap (*(p+2), *(p+5));
  std::swap (*(p+3), *(p+4));
}

template <typename T>
constexpr void byte_swap(T& val, const size_tag<16u>*) noexcept {
  std::byte* p = cast_ptr_to<std::byte>(&val);
  std::swap (*(p+0), *(p+15));
  std::swap (*(p+1), *(p+14));
  std::swap (*(p+2), *(p+13));
  std::swap (*(p+3), *(p+12));
  std::swap (*(p+4), *(p+11));
  std::swap (*(p+5), *(p+10));
  std::swap (*(p+6), *(p+9));
  std::swap (*(p+7), *(p+8));
}

} // end namespace detail

/**
 * @brief Unconditional in-place byte swapping of integral types.
 *
 * The @c byte_swap functions do not check for endianness, so conditional swapping based on
 * endianness should be performed at a higher level.
 *
 * This function template dispatches on specific sizes. If an unsupported size is attempted
 * to be swapped, a static error is generated.
 *
 * @note Floating point swapping is not supported, only integral types. Swapping floating
 * point types can easily result in NaN representations, which can generate hardware traps,
 * either causing runtime crashes or silently changing bits within the floating point number.
 * In particular, returning a byte swapped floating point type by value will result in
 * bad things happening.
 *
 * @note The @c constexpr keyword implicitly declares a function as inline.
 *
 */
template <typename T>
constexpr void byte_swap(T& val) noexcept {

  static_assert(sizeof(val) == 1u || sizeof(val) == 2u || sizeof(val) == 4u || sizeof(val) == 8u || sizeof(val) == 16u,
    "Size for swapping is not supported.");
  static_assert(std::is_integral<T>::value, "Swapping is only supported for integral types.");

  detail::byte_swap(val, const detail::size_tag<sizeof(val)>*(nullptr));
}


// C++ 20 will contain std::endian, which allows full compile time endian detection; in
// addition, C++ 20 std::swap will be constexpr which allow additional compile time
// optimizations; until then, the endian detection and branching will be runtime

inline bool detect_big_endian () {
  const std::uint32_t orig = 0xDDCCBBAA;
  std::uint32_t tmp { orig };
  byte_swap(tmp);
  return tmp == orig;
}

// while the following is not computed at runtime, it will be computed at global
// initialization time

const bool big_endian = detect_big_endian();

} // end namespace

#endif

