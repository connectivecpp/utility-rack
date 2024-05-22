/** @file
 * 
 * @brief Utility functions for conveniently casting a pointer between types, primarily 
 * from some type T to either a @c char* or @c std::byte*.
 * 
 * C++ @c reinterpret_cast can cast any pointer to another pointer type, but
 * the behaviour can be undefined, depending on the access and usage. C++ guarantees 
 * that a pointer can be safely cast to @c void* and then to a @c char* (or 
 * @c std::byte*, which is one of the @c char* types), and that pointer can be cast 
 * back to the original pointer type. Casting between two unrelated types (outside of 
 * the @c void* and @c char* conversions already mentioned) results in undefined behavior 
 * (even if functionally the memory layout is the same and execution is successful on a 
 * given platform).
 *  
 * Both const and non-const overloads are available.
 * 
 * @authors Thurman Gillespy, Cliff Green
 * 
 * @copyright (c) 2019-2024 by Thurman Gillespy, Cliff Green
 * 
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef CAST_PTR_TO_HPP_INCLUDED
#define CAST_PTR_TO_HPP_INCLUDED

namespace chops {

template <typename Dst, typename Src>
const Dst* cast_ptr_to (const Src* p) noexcept {
    return static_cast<const Dst*> (static_cast<const void*> (p));
}

template <typename Dst, typename Src>
Dst* cast_ptr_to (Src* p) noexcept {
    return static_cast<Dst*> (static_cast<void*> (p));
}

} // end namespace

#endif

