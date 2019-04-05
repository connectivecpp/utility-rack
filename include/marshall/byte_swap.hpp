/** @file
 *
 *  @defgroup marshall_module Class and functions for data marshalling (transform
 *  objects into and out of byte streams for transmission over a network or file IO).
 *
 *  @ingroup marshall_module
 *
 *  @brief Byte Extract fundamental integral values from a byte buffer, and the converse
 *  process of taking a fundamental value and pushing it on to a buffer of bytes in network
 *  endian order.
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

// since function template partial specialization is not supported (yet) in C++, overload the 
// extract_val_swap and noswap function templates with a parameter corresponding to the 
// sizeof the value; no data is passed in the second parameter, only using the type for overloading

template <unsigned int>
struct size_tag { };

// if char / byte, no swap needed
template <typename T>
constexpr T extract_val_swap(const std::byte* buf, const size_tag<1u>*) noexcept {
  return static_cast<T>{*buf}; // some char type
}
template <typename T>
constexpr T extract_val_noswap(const std::byte* buf, const size_tag<1u>*) noexcept {
  return static_cast<T>{*buf}; // some char type
}

template <typename T>
constexpr T extract_val_swap(const std::byte* buf, const size_tag<2u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+1);
  *(p+1) = *(buf+0);
  return tmp;
}

template <typename T>
constexpr T extract_val_noswap(const std::byte* buf, const size_tag<2u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+0);
  *(p+1) = *(buf+1);
  return tmp;
}

template <typename T>
constexpr T extract_val_swap(const std::byte* buf, const size_tag<4u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+3);
  *(p+1) = *(buf+2);
  *(p+2) = *(buf+1);
  *(p+3) = *(buf+0);
  return tmp;
}

template <typename T>
constexpr T extract_val_noswap(const std::byte* buf, const size_tag<4u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+0);
  *(p+1) = *(buf+1);
  *(p+2) = *(buf+2);
  *(p+3) = *(buf+3);
  return tmp;
}

template <typename T>
constexpr T extract_val_swap(const std::byte* buf, const size_tag<8u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+7);
  *(p+1) = *(buf+6);
  *(p+2) = *(buf+5);
  *(p+3) = *(buf+4);
  *(p+4) = *(buf+3);
  *(p+5) = *(buf+2);
  *(p+6) = *(buf+1);
  *(p+7) = *(buf+0);
  return tmp;
}

template <typename T>
constexpr T extract_val_noswap(const std::byte* buf, const size_tag<8u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+0);
  *(p+1) = *(buf+1);
  *(p+2) = *(buf+2);
  *(p+3) = *(buf+3);
  *(p+4) = *(buf+4);
  *(p+5) = *(buf+5);
  *(p+6) = *(buf+6);
  *(p+7) = *(buf+7);
  return tmp;
}

} // end namespace detail

// C++ 20 will contain std::endian, which allows full compile time endian detection;
// until then, the endian detection and branching will be runtime, although it
// can be computed at global initialization instead of each time it is called

inline bool detect_big_endian () noexcept {
  const std::uint32_t tmp = 0xDDCCBBAA;
  return *(cast_ptr_to<std::byte>(&tmp) + 3) == static_cast<std::byte>(0xDD);
}

// should be computed at global initialization time

const bool big_endian = detect_big_endian();

/**
 * @brief Extract a value in network byte order (big endian) from a @c std::byte buffer 
 * into a fundamental integral type in native endianness.
 *
 * This function template dispatches on specific sizes. If an unsupported size is attempted
 * to be swapped, a static error is generated.
 *
 * @param buf Buffer of @c std::bytes containing an object of type T in network byte order.
 *
 * @return A value in native endian order.
 *
 * @pre The buffer must contain at least @c sizeof(T) bytes.
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
constexpr T extract_val(const std::byte* buf) noexcept {

  static_assert(sizeof(val) == 1u || sizeof(val) == 2u || sizeof(val) == 4u || sizeof(val) == 8u,
    "Size for swapping is not supported.");
  static_assert(std::is_integral<T>::value, "Swapping is only supported for integral types.");

  return big_endian ? detail::extract_val_noswap(buf, const detail::size_tag<sizeof(T)>*(nullptr)),
                      detail::extract_val_swap(buf, const detail::size_tag<sizeof(T)>*(nullptr));
}

} // end namespace

#endif

