/** @file
 *
 *  @defgroup marshall_module Class and functions for binary data marshalling (transform
 *  objects into and out of byte streams for transmission over a network or file IO).
 *
 *  @ingroup marshall_module
 *
 *  @brief Classes and functions to transform objects into a binary stream of bytes and
 *  the converse, transform a stream of bytes into objects.
 *
 *  Wire protocols that are in full text mode do not need to deal with binary endian
 *  swapping. However, sending or receiving data in a binary form is often desired 
 *  for size efficiency (e.g. sending images or video or large data sets).
 *
 *  These marshalling classes and functions transform application objects into a 
 *  @c std::byte buffer (and the converse), keeping the binary representation. The byte 
 *  buffer binary elements are in network (big endian) order. 
 *
 *  For example, a 32-bit binary number (either a signed or unsigned integer) in native
 *  endian order will be transformed into four 8-bit bytes in network (big) endian order
 *  for sending over a network (or for file I/O). Conversely, the four 8-bit bytes in
 *  network endian order will be transformed back into the original 32-bit binary number
 *  when received (or read as file I/O).
 *
 *  Facilities are provided for fundamental types, including @c bool, as well as vocabulary 
 *  types such as @c std::string and @c std::optional.
 *
 *  Facilities are also provided for sequences, where the number of elements is placed
 *  before the element sequence. The number of bits for the element count is a template
 *  parameter. The same type of size specification is also provided for @c bool (i.e. the
 *  size of the bool value can be specified as 8-bit, 16-bit, etc). @c bool values 
 *  are converted into a 0 or 1 value as appropriate.
 *
 *  @note No support is directly provided for higher level abstractions such as inheritance
 *  hierarchies, version numbers, type flags, or object relations. Pointers are also not 
 *  directly supported (which would typically be part of an object relation).
 *
 *  @note No support is provided for little-endian in the byte buffer. No support is provided
 *  for mixed endian (big-endian with little-endian) or where the endianness is specified as a 
 *  type parameter. No support is provided for "in-place" swapping of values. All of these
 *  use cases can be implemented using other libraries such as Boost.Endian.
 *
 *  @note No direct support is provided for bit fields. A dynamic bit buffer (i.e. where 
 *  individual bits can be appended or extracted) would need to be accessible as a buffer of 
 *  bytes, which can then be directly sent and received.
 * 
 *  @note Floating point types are not supported, only integral types. Character types
 *  are integral types, with no endian swapping needed. Swapping floating point types can 
 *  easily result in NaN representations, which can generate hardware traps, either causing 
 *  runtime crashes or silently changing bits within the floating point number. For this
 *  reason, floating point numbers must either be converted to a string representation, or
 *  converted into an integral type (e.g. an integer with an implicit scale factor).
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2019 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef MARSHALL_HPP_INCLUDED
#define MARSHALL_HPP_INCLUDED

#include "utility/cast_ptr_to.hpp"

#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint32_t, etc
#include <utility> // std::swap
#include <type_traits> // std::is_integral

namespace chops {

namespace detail {

} // end namespace detail

/**
 * @brief Extract a value in network byte order (big endian) from a @c std::byte buffer 
 * into a fundamental integral type in native endianness, swapping bytes as needed.
 *
 * This function template dispatches on specific sizes. If an unsupported size is attempted
 * to be swapped, a compile time error is generated.
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
 */
template <typename T>
T extract_val(const std::byte* buf) noexcept {

  static_assert(sizeof(T) == 1u || sizeof(T) == 2u || sizeof(T) == 4u || sizeof(T) == 8u,
    "Size for value extraction is not supported.");
  static_assert(std::is_integral<T>::value, "Value extraction is only supported for integral types.");

  return big_endian ? detail::extract_val_noswap<T>(buf, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr)) :
                      detail::extract_val_swap<T>(buf, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr));
}

/**
 * @brief Append a fundamental integral value to a @c std::byte buffer, swapping into network
 * endian order (big endian) as needed.
 *
 * This function template dispatches on specific sizes. If an unsupported size is attempted
 * to be swapped, a static error is generated.
 *
 * @param buf Buffer of @c std::bytes already allocated to hold the bytes of val.
 *
 * @param val Value in native endian order to append to buf.
 *
 * @pre The buffer must already be allocated to hold @c sizeof(T) bytes.
 *
 * @note See note above about floating point values, which are not supported.
 *
 */
template <typename T>
void append_val(std::byte* buf, const T& val) noexcept {

  static_assert(sizeof(T) == 1u || sizeof(T) == 2u || sizeof(T) == 4u || sizeof(T) == 8u,
    "Size for value appending is not supported.");
  static_assert(std::is_integral<T>::value, "Value appending is only supported for integral types.");

  if (big_endian) {
    detail::append_val_noswap(buf, val, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr));
  }
  else {
    detail::append_val_swap(buf, val, static_cast<const detail::size_tag<sizeof(T)>* >(nullptr));
  }
}

} // end namespace

#endif

