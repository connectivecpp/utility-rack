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
 *  Functionality is provided for fundamental types, including @c bool, as well as vocabulary 
 *  types such as @c std::string and @c std::optional.
 *
 *  Functionality is also provided for sequences, where the number of elements is placed
 *  before the element sequence. The number of bits for the element count is a template
 *  parameter. The same type of size specification is also provided for @c bool (i.e. the
 *  size of the bool value can be specified as 8-bit, 16-bit, etc). Native @c bool values 
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
 * @brief Extract a sequence in network byte order from a @c std::byte buffer into the
 * provided container.
 *
 * Fill a container with a sequence of elements. See @c append_sequence for additional
 * comments on sequence.
 *
 * @param buf Buffer of @c std::bytes containing a sequence of objects to be extracted,
 * with the count in front.
 *
 * @tparam Cnt Type of the count value, which prepends the sequence of elements. The type
 * determines the size (in bytes) of the count value (e.g. @c std::uint16_t would specify
 * a 16-bit unsigned count).
 *
 * @tparam Container Container type, which must provide a @c value_type declaration, a
 * default constructor, an @c emplace_back method, and a copy or move constructor.
 *
 * @return A container with the element sequence.
 *
 * @pre The buffer must contain at least @c sizeof(T) bytes.
 *
 */
template <typename Cnt, typename Container>
Container extract_sequence(const std::byte* buf) noexcept(fill in) {
  Cnt c = extract_val<Cnt>(buf);

}


/**
 * @brief Append a sequence of elements, including the count, into a @c std::byte buffer using 
 * the lower level @c append_val function.
 *
 * A sequence is defined as an array of elements of type T, along with the number of 
 * elements. When appending to a stream of @c std::bytes, the count is first converted
 * into network byte order and then appended, then the same performed for each element.
 * The number of bits in the count is specified as the type of the count. For example a 
 * @c std::vector of 16-bit @c ints with an 8-bit count can be appended to the buffer as
 * an 8-bit integer, and then successive 16-bit integers.
 *
 * @param buf Buffer of @c std::bytes containing an object of type T in network byte order.
 *
 * @param cnt Number of elements in the sequence.
 *
 * @param start Iterator pointing to the start of the sequence.
 *
 * @param end Iterator pointing to the end of the sequence.
 *
 * @pre The buffer must be large enough to contain the size of the count, plus all of 
 * the elements in the sequence.
 *
 */
template <typename Cnt, typename Iter>
void append_sequence(std::byte* buf, Cnt cnt, Iter start, Iter end) noexcept {
  append_val(buf, cnt);
  while (start != end) {
    append_val(buf, *start);
    ++start;
  }
}



} // end namespace

#endif

