/** @file
 * 
 *  @defgroup utility_module
 * 
 *  @brief Utility functions for safely casting a pointer.
 * 
 *  C++ @c reinterpret_cast can cast any pointer to another pointer type, but
 *  the behaviour can be undefined. C++ guarentees that a pointer can be safely
 *  cast to either @c void* or @c char*. That pointer can be cast to another
 *  type, or cast back to the original pointer type.
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


template <typename Dst, typename Src>
const Dst* cast_ptr_to (const Src* p) {
    return static_cast<const Dst*> (static_cast<const void*> (p));
}

template <typename Dst, typename Src>
Dst* cast_ptr_to (Src* p) {
    return static_cast<Dst*> (static_cast<void*> (p));
}