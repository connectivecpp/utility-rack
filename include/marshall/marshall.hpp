/** @file
 *
 *  @defgroup marshall_module Classes and functions for binary data marshalling and
 *  unmarshalling (transform objects into and out of byte streams for transmission over 
 *  a network or file IO).
 *
 *  @ingroup marshall_module
 *
 *  @brief Classes and functions to transform objects into a binary stream of bytes 
 *  (marshall) and the converse (unmarshall), transform a stream of bytes into objects.
 *
 *  The marshalling classes and functions are designed for networking (or file I/O), 
 *  where binary data marshalling and unmarshalling is needed to send and receive 
 *  messages (or to write or read defined portions of a file). Application code using 
 *  this library has full control of every byte that is sent or received. Application 
 *  objects are transformed into a @c std::byte buffer (and the converse) keeping a 
 *  binary representation in network (big endian) order.
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
 *  directly supported (which would typically be part of an object relation). These higher
 *  level abstractions as well as "saving and later restoring a full application state" 
 *  are better served by a library such as Boost.Serialization.
 *
 *  There is not any automatic generation of message processing code, such as Google 
 *  Protocol Buffers (a language neutral message definition process that generates 
 *  marshalling and unmarshalling code). Future C++ standards supporting reflection 
 *  may allow higher abstractions and more automation of marshalling code, but this
 *  library provides a modern C++ API (post C++ 11) for direct control of the 
 *  byte buffers. In particular, all of the build process complications required for
 *  code generation are not present in this (header only) library.
 *
 *  Wire protocols that are in full text mode do not need to deal with binary endian
 *  swapping. However, sending or receiving data in a binary form is often desired 
 *  for size efficiency (e.g. sending images or video or large data sets).
 *
 *  Functionality is provided for fundamental types, including @c bool, as well as vocabulary 
 *  types such as @c std::string and @c std::optional. @c std::variant and @c std::any
 *  require value extraction by the application and are not directly supported by this
 *  library. (This might be a future enhancement if a good design is proposed.) Support is 
 *  also provided for sequences, where the number of elements is placed before the element 
 *  sequence. 
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
 *  use cases can be implemented using other libraries such as Boost.Endian.
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
#include "marshall/shared_buffer.hpp"
#include "marshall/extract_append.hpp"

#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint32_t, etc
#include <optional>

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
 * @return Total number of bytes appended to the @c std::byte buffer.
 *
 * @pre The buffer must be large enough to contain the size of the count, plus all of 
 * the elements in the sequence.
 *
 */
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

/**
 * @brief Adapt a C-style array or @c std::array so that it can be used with the
 * @c chops::marshaller class template.
 *
 * This class provides four methods (@c size, @c resize, @c data, @c clear) as required by the
 * @c chops::marshaller buf parameter. This adapter can only be used where the array or @c std::array
 * object lifetime is the same or greater than the @c buf_adapter object, and where the 
 * array address never changes. In other words, a @c std::vector<std::byte> cannot be used with
 * this adapter, as the underlying array may be reallocated and the buffer address changed 
 * (instead, a @c std::vector<std::byte> can be directly used with the @c chops::marshaller class 
 * template).
 *
 */
class buf_adapter {
public:
/**
 * @brief Construct the @ buf_adapter given a @c std::byte pointer to the beginning of an array.
 *
 * @pre The @c std::byte pointer passed in to the constructor must point to a buffer large enough
 * to contain the full size of the marshalled data. No "end of buffer" checks are performed in the 
 * @c chops::marshaller class template.
 */
  buf_adapter(std::byte* buf) : m_buf(buf), m_size(0u) noexcept { }

/**
 * @brief Return the size of the data which has been written into the buffer.
 *
 * @return The size of the data which has been written into the buffer, which is modified through
 * the @c resize method.
 *
 */
  std::size_t size() const noexcept {
    return m_size;
  }
/**
 * @brief Track how many bytes have been written into the buffer.
 */
  void resize(std::size_t sz) noexcept {
    m_size = sz;
  }
/**
 * @brief Return a pointer to the beginning of the buffer.
 *
 * @return Return a pointer to the beginning of the buffer.
 */
  std::byte* data() noexcept {
    return m_buf;
  }
/**
 * @brief Logically reset so that new data can be written into the buffer.
 */
  void clear() noexcept {
    m_size = 0u;
  }
  
private:
  std::byte* m_buf;
  std::size_t m_size;
};

/**
 * @brief Blah
 *
 * @tparam Buf A container storing a contiguous @c std::byte buffer. The container must 
 * support the following methods: @c resize, @c size, @c data, and @c clear. A 
 * @c std::vector<std::byte> is supported, as well as the @c chops::mutable_shared_buffer
 * class. A C-style array or @c std::array is supported by using the @c chops::buf_adapter
 * class. While @c std::basic_string supports the four needed methods, it is unknown if
 * it is defined behavior to instantiate a @c std::basic_string with @c std::byte (and is
 * not recommended according to Arne Metz at https://arne-mertz.de/2018/11/string-not-for-raw-data/).
 *
 * @param cnt Number of elements in the sequence.
 *
 * @param start Iterator pointing to the start of the sequence.
 *
 * @param end Iterator pointing to the end of the sequence.
 *
 * @pre The @c std::byte buffer passed in to the constructor must be able to contain the full size 
 * of the marshalled data, whether through expansion of the buffer or a buffer already pre-allocated.
 */
template <typename Buf = mutable_shared_buffer>
class marshaller {
public:


/**
 * @brief blah
 * the lower level @c append_val function.
 *
 * @param buf blah
 *
 * @param cnt Number of elements in the sequence.
 *
 * @param start Iterator pointing to the start of the sequence.
 *
 * @param end Iterator pointing to the end of the sequence.
 *
 * @pre The @c std::byte buffer passed in to the constructor must be able to contain the full size 
 * of the marshalled data, whether through expansion of the buffer or a buffer already pre-allocated.
 */
  marshaller (Buf& buf) : m_buf(buf), m_offset(0u) noexcept { }

/**
 * @brief Copy construction is not allowed.
 */
  marshaller(const marshaller&) = delete;

  template <typename Cast, typename T>
  marshaller<Buf>& marshall(const T& val) {
    m_buf.resize(m_buf.size() + sizeof(Cast));
    m_offset += append_val(m_buf.data()+m_offset, static_cast<Cast>(val));
    return *this;
  }

  std::size_t size() const noexcept {
    return m_offset;
  }

  void clear() {
    m_buf.clear();
    m_offset = 0u;
  }

private:
  Buf& m_buf;
  std::size_t m_offset; // offset is used instead of pointer since a buffer resize or clear may 
                        // reallocate memory, invalidating the pointer
};

// various overloaded marshall functions, each taking a marshaller object

template <typename Cast, typename T, typename Buf = mutable_shared_buffer>
marshaller<Buf>& marshall(marshaller<Buf>& mler, const T& val) {
  return mler.marshall<Cast>(val);
}

template <typename Cast, typename Buf = mutable_shared_buffer>
marshaller<Buf>& marshall(marshaller<Buf>& mler, bool b) {
  return mler.marshall<Cast>(static_cast<Cast>(b ? 1 : 0));
}

template <typename CastBool, typename CastVal, typename T, typename Buf = mutable_shared_buffer>
marshaller<Buf>& marshall(marshaller<Buf>& mler, const std::optional<T>& val) {
  marshall<CastBool>(mler, val.has_value());
  if (val.has_value()) {
    marshall<CastVal>(*val);
  }
  return mler;
}

template <typename CntCast, typename Iter, typename Buf = mutable_shared_buffer>
marshaller<Buf>& marshall(marshaller<Buf>& mler, std::size_t num, Iter start) {
  marshall<CntCast>(num);
  repeat(num, [&mler] { marshall
  return mler;
}

} // end namespace

#endif

