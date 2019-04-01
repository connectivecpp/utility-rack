/** @file
 * 
 *  @defgroup utility_module Simple utility classes and functions.
 *
 *  @ingroup utility_module
 * 
 *  @brief Utility functions for safely casting a pointer between types, primarily 
 *  from some type T to either a @c char* or @c std::byte*.
 * 
 *  C++ @c reinterpret_cast can cast any pointer to another pointer type, but
 *  the behaviour can be undefined. C++ guarentees that a pointer can be safely
 *  cast to either @c void* or @c char* (or @c std::byte*, which is a form of @c char*), 
 *  and that pointer can be cast back to the original pointer type. Casting between
 *  two unrelated types (outside of the @c void* and @c char* conversions already 
 *  mentioned) results in undefined behavior (even if functionally the memory layout is 
 *  the same and execution is successful on a given platform).
 *  
 *  Both const and non-const overloads are available.
 * 
 *  @authors Thurman Gillespy, Cliff Green
 * 
 *  Copyright (c) 2019 by Thurman Gillespy, Cliff Green
 * 
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 */

#ifndef CAST_PTR_TO_HPP_INCLUDED
#define CAST_PTR_TO_HPP_INCLUDED

namespace chops {

template <typename Dst, typename Src>
const Dst* cast_ptr_to (const Src* p) {
    return static_cast<const Dst*> (static_cast<const void*> (p));
}

template <typename Dst, typename Src>
Dst* cast_ptr_to (Src* p) {
    return static_cast<Dst*> (static_cast<void*> (p));
}

} // end namespace

#endif

