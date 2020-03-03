/** @file
 *
 *  @defgroup timer_module Classes and functions for timer functionality.
 *
 *  @ingroup timer_module
 *
 *  @brief An asynchronous periodic timer providing both duration and timepoint 
 *  based periods.
 *
 *  Writing code using asynchronous timers from the Asio library is relatively easy. 
 *  However, there are no timers that are periodic. This class simplifies the task, 
 *  using application supplied function object callbacks.
 *
 *  A @c periodic_timer stops when the application supplied function object 
 *  returns @c false rather than @c true.
 *
 *  A periodic timer can be used as a "one-shot" timer by finishing after 
 *  one invocation (i.e. unconditionally return @c false from the function 
 *  object).
 *
 *  @note This class does not perform "this" reference counting. It is up to 
 *  the application code to guarantee that a @c periodic_timer has not been 
 *  destructed before handlers (function object callbacks) are invoked.
 *
 *  A common idiom is to use @c std::enable_shared_from_this, call 
 *  @c std::shared_from_this, and store the result in the function object 
 *  callback object.
 *  
 *  @note @c std::chrono facilities seem to be underspecified on @c noexcept,
 *  very few of the functions in @c periodic_timer are @c noexcept.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2017-2019 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef PERIODIC_TIMER_HPP_INCLUDED
#define PERIODIC_TIMER_HPP_INCLUDED

#include "asio/basic_waitable_timer.hpp"
#include "asio/io_context.hpp"

#include <chrono>
#include <system_error>
#include <utility> // std::move, std::forward

namespace chops {

template <typename Clock = std::chrono::steady_clock>
class periodic_timer {
public:

  using duration = typename Clock::duration;
  using time_point = typename Clock::time_point;

private:

  asio::basic_waitable_timer<Clock> m_timer;

private:
  template <typename F>
  void duration_handler_impl(const time_point& last_tp, const duration& dur, 
                             const std::error_code& err, F&& func) {
    time_point now_time { Clock::now() };
    // pass err and elapsed time to app function obj
    if (!func(err, now_time - last_tp) || 
        err == asio::error::operation_aborted) {
      return; // app is finished with timer for now or timer was cancelled
    }
    m_timer.expires_after(dur);
    m_timer.async_wait( [now_time, dur, f = std::move(func), this]
            (const std::error_code& e) {
        duration_handler_impl(now_time, dur, e, std::move(f));
      }
    );
  }
  template <typename F>
  void timepoint_handler_impl(const time_point& last_tp, const duration& dur, 
                              const std::error_code& err, F&& func) {
    // pass err and elapsed time to app function obj
    if (!func(err, (Clock::now() - last_tp)) || 
        err == asio::error::operation_aborted) {
      return; // app is finished with timer for now or timer was cancelled
    }
    m_timer.expires_at(last_tp + dur + dur);
    m_timer.async_wait( [f = std::move(func), last_tp, dur, this]
            (const std::error_code& e) {
        timepoint_handler_impl(last_tp+dur, dur, e, std::move(f));
      }
    );
  }

public:

  /**
   * Construct a @c periodic_timer with an @c io_context. Other information such as duration 
   * will be supplied when @c start is called.
   *
   * Constructing a @c periodic_timer does not start the actual timer. Calling one of the 
   * @c start methods starts the timer.
   *
   * The application supplied function object for any of the @c start methods requires the 
   * following signature:
   * @code
   *   bool (std::error_code, duration);
   * @endcode
   *
   * The @c duration parameter provides an elapsed time from the previous callback.
   *
   * The clock for the asynchronous timer defaults to @c std::chrono::steady_clock.
   * Other clock types can be used if desired (e.g. @c std::chrono::high_resolution_clock 
   * or @c std::chrono::system_clock). Note that some clocks allow time to be externally 
   * adjusted, which may influence the interval between the callback invocation.
   *
   * Move semantics are allowed for this type, but not copy semantics. When a move 
   * construction or move assignment completes, all timers are cancelled with 
   * appropriate notification, and @c start will need to be called.
   *
   * @param ioc @c io_context for asynchronous processing.
   *
   */
  explicit periodic_timer(asio::io_context& ioc) noexcept : m_timer(ioc) { }

  periodic_timer() = delete; // no default ctor

  // disallow copy construction and copy assignment
  periodic_timer(const periodic_timer&) = delete;
  periodic_timer& operator=(const periodic_timer&) = delete;

  // allow move construction and move assignment
  periodic_timer(periodic_timer&&) = default;
  periodic_timer& operator=(periodic_timer&& rhs) {
    m_timer.cancel();
    m_timer = std::move(rhs.m_timer);
  }

  // modifying methods

  /**
   * Start the timer, and the application supplied function object will be invoked 
   * after an amount of time specified by the duration parameter.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param func Function object to be invoked. 
   *
   */
  template <typename F>
  void start_duration_timer(const duration& dur, F&& func) {
    m_timer.expires_after(dur);
    m_timer.async_wait( [dur, f = std::move(func), this] (const std::error_code& e) {
        duration_handler_impl(Clock::now(), dur, e, std::move(f));
      }
    );
  }
  /**
   * Start the timer, and the application supplied function object will be invoked 
   * first at a specified time point, then afterwards as specified by the duration 
   * parameter.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param when Time point when the first timer callback will be invoked.
   *
   * @param func Function object to be invoked.
   *
   */
  template <typename F>
  void start_duration_timer(const duration& dur, const time_point& when, F&& func) {
    m_timer.expires_at(when);
    m_timer.async_wait( [dur, f = std::move(func), this] (const std::error_code& e) {
        duration_handler_impl(Clock::now(), dur, e, std::move(f));
      }
    );
  }
  /**
   * Start the timer, and the application supplied function object will be invoked 
   * on timepoints with an interval specified by the duration.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param func Function object to be invoked. 
   *
   */
  template <typename F>
  void start_timepoint_timer(const duration& dur, F&& func) {
    start_timepoint_timer(dur, (Clock::now() + dur), std::forward<F>(func));
  }
  /**
   * Start the timer on the specified timepoint, and the application supplied function object 
   * will be invoked on timepoints with an interval specified by the duration.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param when Time point when the first timer callback will be invoked.
   *
   * @param func Function object to be invoked. 
   *
   * @note The elapsed time for the first callback invocation is artificially set to the 
   * duration interval.
   */
  template <typename F>
  void start_timepoint_timer(const duration& dur, const time_point& when, F&& func) {
    m_timer.expires_at(when);
    m_timer.async_wait( [when, dur, f = std::move(func), this]
            (const std::error_code& e) {
        timepoint_handler_impl((when-dur), dur, e, std::move(f));
      }
    );
  }

  /**
   * Cancel the timer. The application function object will be called with an 
   * "operation aborted" error code.
   *
   * A cancel may implicitly be called if the @c periodic_timer object is move copy 
   * constructed or move assigned.
   */
  void cancel() {
    m_timer.cancel();
  }
};

} // end namespace

#endif

