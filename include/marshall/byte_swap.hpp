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

#include <cstddef> // std::byte, std::size_t
#include <utility> // std::swap

#include "utility/cast_ptr_to.hpp"

namespace chops {

/**
 * @brief Unconditional in-place byte swapping.
 *
 * The @c byte_swap functions do not check for endianness, so conditional swapping based on
 * endianness should be performed at a higher level.
 *
 * This function template is overloaded for specific sizes. If none match, a compile time
 * error is generated.
 *
 * @note The @c constexpr keyword implicitly declares a function as inline.
 *
 */
template <typename T>
constexpr void byte_swap(T& val) noexcept {
  byte_swap_impl(val, sizeof(val);
}



namespace detail {

  static_assert("Size for swapping is not supported.");
}


// if char / byte, no swap needed
template <1u, typename T>
constexpr void byte_swap(T&) {
}

template <2u, typename T>
constexpr void byte_swap(T& val) {
  std::byte* p = cast_ptr_to<std::byte>(&val);
  std::swap (*(p+0), *(p+1));
}

template <4u, typename T>
constexpr void byte_swap(T& val) {
  std::byte* p = cast_ptr_to<std::byte>(&val);
  std::byte tmp = *p;
  *p = *(p+1);
  *(p+1) = tmp;
}

} // end namespace

#endif

