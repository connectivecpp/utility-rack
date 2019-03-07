/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for @c periodic_timer class template.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2017-2018 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER

#include "catch2/catch.hpp"


#include <chrono>
#include <thread>
#include <optional>
#include <system_error>

#include "asio/executor_work_guard.hpp"
#include "asio/executor.hpp"

#include "timer/periodic_timer.hpp"
#include "utility/repeat.hpp"

constexpr int Expected = 9;
int count = 0;

template <typename D>
bool lambda_util (std::error_code err, D elap) {
  ++count;
  INFO ("count = " << count << ", err code = " << err.value() << ", " << err.message());
  return count < Expected;
}

using wk_guard = asio::executor_work_guard<asio::io_context::executor_type>;

void wait_util (std::chrono::milliseconds ms, wk_guard& wg, std::thread& thr) {
  std::this_thread::sleep_for(ms);
  wg.reset();
  thr.join();
}


template <typename Clock>
void test_util () {

  GIVEN ( "A clock and a duration") {

    asio::io_context ioc;
    chops::periodic_timer<Clock> timer {ioc};
    wk_guard wg { asio::make_work_guard(ioc) };

    std::thread thr([&ioc] () { ioc.run(); } );
    count = 0;

    WHEN ( "The duration is 100 ms" ) {
      auto test_dur { 100 };
      timer.start_duration_timer(std::chrono::milliseconds(test_dur),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util (std::chrono::milliseconds((Expected+1)*test_dur), wg, thr);

      THEN ( "the timer callback count should match expected") {
        REQUIRE (count == Expected);
      }
    }
    WHEN ( "The duration is 200 ms and the start time is 2 seconds in the future" ) {
      auto test_dur { 200 };
      timer.start_duration_timer(std::chrono::milliseconds(test_dur), Clock::now() + std::chrono::seconds(2),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util(std::chrono::milliseconds((Expected+1)*test_dur + 2000), wg, thr);

      THEN ( "the timer callback count should match expected") {
        REQUIRE (count == Expected);
      }
    }
    WHEN ( "The duration is 100 ms and the timer pops on timepoints" ) {
      auto test_dur { 100 };
      timer.start_timepoint_timer(std::chrono::milliseconds(test_dur),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util (std::chrono::milliseconds((Expected+1)*test_dur), wg, thr);

      THEN ( "the timer callback count should match expected") {
        REQUIRE (count == Expected);
      }
    }
    WHEN ( "The duration is 200 ms and the timer pops on timepoints starting 2 seconds in the future" ) {
      auto test_dur { 200 };
      timer.start_timepoint_timer(std::chrono::milliseconds(test_dur), Clock::now() + std::chrono::seconds(2),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util(std::chrono::milliseconds((Expected+1)*test_dur + 2000), wg, thr);

      THEN ( "the timer callback count should match expected") {
        REQUIRE (count == Expected);
      }
    }

  } // end given
}

SCENARIO ( "A periodic timer can be instantiated on the steady clock", "[periodic_timer] [steady_clock]" ) {

  test_util<std::chrono::steady_clock>();

}
SCENARIO ( "A periodic timer can be instantiated on the system clock", "[periodic_timer] [system_clock]" ) {

  test_util<std::chrono::system_clock>();

}
SCENARIO ( "A periodic timer can be instantiated on the high resolution clock", "[periodic_timer] [high_resolution_clock]" ) {

  test_util<std::chrono::high_resolution_clock>();

}

