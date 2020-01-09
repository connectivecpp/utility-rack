/** @file
 *
 *  @defgroup marshall_module Classes and functions for big-endian binary data 
 *  marshalling and unmarshalling (transform objects into and out of byte streams 
 *  for transmission over a network or for file IO).
 *
 *  @ingroup marshall_module
 *
 *  @brief Classes and functions to transform objects into a big-endian binary stream 
 *  of bytes (marshall) and the converse (unmarshall), transform a stream of bytes into 
 *  objects.
 *
 *  The @c utility-rack @c marshall and @c unmarshall functions and classes provide a 
 *  simple and light abstraction for binary big-endian serialization. There are no
 *  message or element definitions, no embedded preprocesser syntax, and no extra 
 *  build steps.
 *
 *  These facilities are useful when explicit control of every bit and byte is needed 
 *  (and the wire protocol format is big-endian). Other marshalling and serialization
 *  designs have strengths and weaknesses (see higher level documentation for more
 *  explanation).
 *
 *  @note The design of the binary marshall and unmarshall functions is a good fit
 *  for a C++ metaprogamming implementation (using variadic templates). In particular,
 *  the primary design concept is a mapping of two (and sometimes three) types to a 
 *  single value. A typelist would allow a single function (or method) call to operate
 *  on multiple values, instead of being forced to call the @c marshall or @c unmarshall
 *  function once for each value (or sequence). However, the first release uses the 
 *  simpler (no metaprogramming, no variadic templates) implementation with a hope that
 *  a more sophisticated version will be available in the future.
 *
 *  The marshalling classes and functions are designed for networking (or file I/O), 
 *  where binary data marshalling and unmarshalling is needed to send and receive 
 *  messages (or to write or read defined portions of a file). Application code using 
 *  this library has full control of every byte that is sent or received. Application 
 *  objects are transformed into a @c std::byte buffer (and the converse) keeping a 
 *  binary representation in network (big-endian) order.
 *
 *  For example, a 32-bit binary number (either a signed or unsigned integer) in native
 *  endian order will be transformed into four 8-bit bytes in network (big) endian order
 *  for sending over a network (or for file I/O). Conversely, the four 8-bit bytes in
 *  network endian order will be transformed back into the original 32-bit binary number
 *  when received (or read as file I/O). A @c bool can be transformed into either a 8-bit,
 *  16-bit, 32-bit, or 64-bit number of either 1 or 0 (and back). A sequence 
 *  (@c std::vector or array or other container) can be transformed into a count (8-bit, 
 *  16-bit, et al) followed by each element of the sequence. A @c std::optional can be 
 *  transformed into a @c bool (8-bit, 16-bit, et al) followed by the value (if present).
 *
 *  No support is directly provided for higher level abstractions such as inheritance
 *  hierarchies, version numbers, type flags, or object relations. Pointers are also not 
 *  directly supported (which would typically be part of an object relation). No specific
 *  wire protocol or data encoding is specified (other than big-endian). These higher
 *  level abstractions as well as "saving and later restoring a full application state" 
 *  are better served by a library such as Boost Serialization or Google Protocol
 *  Buffers or Cap'n Proto.
 *
 *  There is not any automatic generation of message processing code (e.g. Google 
 *  Protocol Buffers, a language neutral message definition process that generates 
 *  marshalling and unmarshalling code). Future C++ standards supporting reflection 
 *  may allow higher abstractions and more automation of marshalling code, but this
 *  library provides a modern C++ API (post C++ 11) for direct control of the 
 *  byte buffers. In particular, all of the build process complications required for
 *  code generation are not present in this (header only) library.
 *
 *  Wire protocols that are in full text mode do not need to deal with binary endian
 *  swapping. However, sending or receiving data in a binary form is often desired 
 *  for size efficiency (e.g. sending images and video, large data sets, or where
 *  the message size needs to be as small as possible).
 *
 *  Functionality is provided for fundamental types, including @c bool, as well as vocabulary 
 *  types such as @c std::string and @c std::optional. Support is also provided for sequences, 
 *  where the number of elements is placed before the element sequence in the stream of
 *  bytes. 
 *
 *  Application defined types can be associated with a @c marshall and @c unmarshall 
 *  function overload, providing a convenient way to reuse the same lower-level 
 *  marshalling code. Specifically, a type @c MyType can be used in a sequence or in 
 *  a @c std::optional or as part of a higher level @c struct or @c class type without needing 
 *  to duplicate the marshalling calls within the @c MyType @c marshall and @c unmarshall 
 *  functions.
 * 
 *  @c std::variant and @c std::any are not directly supported and require value extraction 
 *  by the application. (Supporting @c std::variant or @c std::any might be a future 
 *  enhancement if a good design is proposed.) @c std::wstring and other non-char strings are 
 *  also not directly supported, and require additional calls from the application.
 *
 *  Central to the design of these marshalling and unmarshalling functions is a mapping of 
 *  two types to a single value. For marshalling, the two types are the native type (e.g. 
 *  @c int, @c short, @c bool), and the type to be used for the marshalling, typically
 *  a fixed width integer type, as specified in the @c <cstdint> header (e.g. 
 *  @c std::uint32_t, @c std::int16_t, @c std::int8_t). For unmarshalling, the same
 *  concept is used, a fixed width integer type that specifies the size in the byte
 *  buffer, and the native type, thus the application would specify that a @c std::int16_t
 *  in the byte buffer will be unmarshalled into an application @c int value.
 *
 *  @note No support is provided for little-endian in the byte buffer. No support is provided
 *  for mixed endian (big-endian with little-endian) or where the endianness is specified as a 
 *  type parameter. No support is provided for "in-place" swapping of values. All of these
 *  use cases can be implemented using other libraries such as Boost Endian.
 *
 *  @note Performance considerations - for marshalling, iterative resizing of the output
 *  buffer is a fundamental operation. @c std::vector and @c mutable_shared_buffer 
 *  @c resize methods use efficient logic for internal buffer allocations (@c mutable_shared_buffer
 *  uses @c std::vector internally). Custom containers used as the buffer parameter should
 *  have similar efficient @c resize method logic. Calling @c reserve at appropriate places may 
 *  provide a small performance increase, at the cost of additional requirements on the buffer
 *  type.
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
#include "marshall/shared_buffer.hpp"
#include "marshall/extract_append.hpp"

#include <cstddef> // std::byte, std::size_t, std::nullptr_t
#include <cstdint> // std::uint32_t, etc
#include <optional>
#include <string>
#include <string_view>
#include <cstring> // std::memcpy
#include <type_traits>
#include <iterator> // value type of iterator
#include <array>
#include <cassert>

#include <iostream> // debugging

namespace chops {

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
/*
template <typename Cnt, typename Container>
Container extract_sequence(const std::byte* buf) noexcept(fill in) {
  Cnt c = extract_val<Cnt>(buf);

}
*/


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
 * @return Total number of bytes appended to the @c std::byte buffer.
 *
 * @pre The buffer must be large enough to contain the size of the count, plus all of 
 * the elements in the sequence.
 *
 */
/*
template <typename Cnt, typename Iter>
std::size_t append_sequence(std::byte* buf, Cnt cnt, Iter start, Iter end) noexcept {
  std::size_t num = 0;
  append_val(buf, cnt);
  num += cnt;
  while (start != end) {
    num += append_val(buf, *start);
    ++start;
  }
}
*/

/**
 * @brief Adapt a @c std::array so that a fixed size @c std::byte array can be used with 
 * the @c chops::marshall function template as the @c Buf template parameter.
 *
 * This class provides three methods (@c size, @c resize, @c data) as required 
 * by the @c Buf parameter type in the @c chops::marshall function template. 
 *
 * The logical size of the buffer is tracked. If @c resize is called and there 
 * is not enough room in the buffer an assert is fired.
 *
 */
template <std::size_t N>
class fixed_size_byte_array {
public:
/**
 * @brief Default construct the @ fixed_size_byte_array.
 *
 */
  fixed_size_byte_array() noexcept : m_buf(), m_size(0u) { }

/**
 * @brief Return the size of the data which has been written into the buffer.
 *
 * @return The size of the data which has been written into the buffer, which is 
 * modified through the @c resize method.
 *
 */
  std::size_t size() const noexcept {
    return m_size;
  }
/**
 * @brief Increase the logical size, allowing bytes to be appended.
 *
 * @param sz New logical size.
 */
  void resize(std::size_t sz) noexcept {
    assert(sz <= N);
    m_size = sz;
  }
/**
 * @brief Return a pointer to the beginning of the buffer.
 *
 * @return A pointer to the beginning of the buffer.
 */
  std::byte* data() noexcept {
    return m_buf.data();
  }
/**
 * @brief Logically reset so that new data can be written into the buffer
 * at the beginning.
 */
  void clear() noexcept {
    m_size = 0u;
  }
  
private:
  std::array<std::byte, N> m_buf;
  std::size_t              m_size;
};


// add a function parameter to the function overload set so that std::vector<std::byte> and 
// similar can be used for the Buf template parameter
struct adl_tag { };

// lower-level function template that performs the actual buffer manipulation and 
// marshalling of a single value, with an ADL tag for full namespace inclusion in the
// overload set; this function template is not called directly by application code, and
// is only used with arithmetic values or a std::byte
template <typename CastValType, typename T, typename Buf = chops::mutable_shared_buffer>
Buf& marshall(Buf& buf, const T& val, adl_tag) {
  auto old_sz = buf.size();
  buf.resize(old_sz + sizeof(CastValType));
  append_val(buf.data()+old_sz, static_cast<CastValType>(val));
  return buf;
}

/**
 * @brief Marshall a single arithmetic value or a @c std::byte into a buffer of bytes.
 *
 * This is the "basic" @c marshall method, handling fundamental arithmetic values (@c char, 
 * @c short, @c int, @c double, @c float, etc) or a @c std::byte, as well as the customization 
 * point for user defined types. This function template calls a lower level function template that
 * expands the buffer and appends the value to the buffer, performing byte swapping into big-endian 
 * format as needed. Single @c char and @c std::byte values will not be byte swapped.
 *
 * Example usage - marshall an @c int as an unsigned 16 bit value:
 * @code
 *   std::vector<std::byte> buf;
 *   // ...
 *   marshall<std::uint16_t>(buf, my_int);
 * @endcode
 *
 * Example floating point usage - marshall a @c double:
 * @code
 *   std::vector<std::byte> buf;
 *   // ...
 *   marshall<double>(buf, my_double);
 * @endcode
 *
 * @tparam CastValType The fixed size type to be used for marshalling in the byte buffer,
 * typically a type such as @c std::int32_t, @c std::uint32_t, @c std::uint16_t, @c float,
 * etc; this type must always be supplied in the function call, since it is not
 * deduced from the function arguments. Note that there are no standard typedefs for floating
 * point types - use @c float or @c double as the casting type. 
 *
 * @tparam T Type of value to be marshalled (typically deduced).
 *
 * @tparam Buf The buffer type, which must contain an array of @c std::bytes, and must support 
 * @c size, @c resize, and @c data methods; @c chops::mutable_shared_buffer,
 * @c std::vector<std::byte>, and @c chops::fixed_size_byte_array all satisfy this
 * requirement.
 *
 * @param buf Buffer to store marshalled value, @c resize will be called to expand buffer.
 *
 * @param val Value to be marshalled.
 *
 */
template <typename CastValType, typename T, typename Buf = chops::mutable_shared_buffer,
          typename = std::enable_if_t<detail::is_arithmetic_or_byte<T>()> >
Buf& marshall(Buf& buf, const T& val) {
  return marshall<CastValType>(buf, val, adl_tag { });
}

// overloads for specific types
template <typename CastBoolType, typename Buf = chops::mutable_shared_buffer>
Buf& marshall(Buf& buf, bool b) {
  return marshall<CastBoolType>(buf, static_cast<CastBoolType>(b ? 1 : 0));
}


template <typename CastBoolType, typename CastValType, typename T, typename Buf = chops::mutable_shared_buffer>
Buf& marshall(Buf& buf, const std::optional<T>& val) {
  marshall<CastBoolType>(buf, val.has_value());
  if (val.has_value()) {
    if constexpr (detail::is_arithmetic_or_byte<T>()) {
      marshall<CastValType>(buf, *val);
    }
    else { // *val should be a UDT
      marshall(buf, *val);
    }
  }
  return buf;
}

// overload for sequences
template <typename CastCntType, typename CastValType, typename Iter, typename Buf = chops::mutable_shared_buffer,
          typename T = typename std::iterator_traits<Iter>::value_type>
Buf& marshall_seq(Buf& buf, std::size_t num_elems, Iter iter) {
  marshall<CastCntType>(buf, num_elems);
  for (std::size_t i = 0u; i < num_elems; ++i) {
    if constexpr (detail::is_arithmetic_or_byte<T>()) {
      marshall<CastValType>(buf, *iter);
    }
    else {
      marshall(buf, *iter);
    }
    ++iter;
  }
  return buf;
}

// efficiently append a buffer of bytes to the end of the existing buffer
template <typename Buf = chops::mutable_shared_buffer>
Buf& marshall_buf(Buf& buf, std::size_t num_bytes, const std::byte* append_buf) {
  auto old_sz = buf.size();
  buf.resize(old_sz + num_bytes);
  std::memcpy (buf.data()+old_sz, append_buf, num_bytes);
  return buf;
}

template <typename CastCntType, typename Buf = chops::mutable_shared_buffer>
Buf& marshall(Buf& buf, std::string_view str) {
  marshall<CastCntType>(buf, str.size());
  return marshall_buf(buf, str.size(), cast_ptr_to<std::byte>(str.data()));
}

template <typename CastCntType, typename Buf = chops::mutable_shared_buffer>
Buf& marshall(Buf& buf, const std::string& str) {
  return marshall<CastCntType>(buf, std::string_view(str));
}

template <typename CastCntType, typename Buf = chops::mutable_shared_buffer>
Buf& marshall(Buf& buf, const char* str) {
  return marshall<CastCntType>(buf, std::string_view(str));
}

} // end namespace

#endif

