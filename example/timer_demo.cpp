/** @file
 *  
 *  @brief Example code demonstrating use of @c chops::periodic_timer
 * 
 *  @ingroup example_module
 *
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/23/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file:
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ \
 *  -I ~/Projects/asio/asio/include/ \
 *  -I ~/Projects/ timer_demo.cpp -lpthread
 */

#include <iostream>
#include <cstdlib> // EXIT_SUCCESS
#include <chrono> // std::chrono
#include <thread> // std::thread

#include "asio/executor_work_guard.hpp"
#include "asio/executor.hpp"

#include "timer/periodic_timer.hpp"

/**
 * @brief The @c chops::periodic_timer calls a user supplied callback 
 * function at a user defined interval. The callback funtion is repeatedly 
 * called while it returns @c true, but the timer terminates when the 
 * function returns @c false.
 * 
 * The timer executes inside an @c std::thread. The thread and timer are linked 
 * with an @c asio::io_context. As the example shows, multiple timers can be 
 * run inside one thread. An @c asio::executor_work_guard controls ownership 
 * of executor work within the thread. Without either the thread or the @c
 * exector_work_guard, the timer will not execute.
 */
int main() {
    using Clock = std::chrono::steady_clock;
    using wk_guard = asio::executor_work_guard<asio::io_context::executor_type>;

    asio::io_context ioc;
    chops::periodic_timer timer1 {ioc};
    chops::periodic_timer timer2 {ioc};
    wk_guard wg { asio::make_work_guard(ioc) };

    std::thread thr([&ioc] () { ioc.run(); } );
    
    constexpr auto dur1 { 500 };
    constexpr auto dur2 { 500 };
    constexpr int num_repeats = 5;
    
    // lambda functions called by timer every dur msec
    auto f1 = [&num_repeats] (std::error_code err, Clock::duration elap) {
        static int i = 0; std::cout << "Hello, "; std::cout.flush();
        return (++i >= num_repeats ? false : true); };
    
    auto f2 = [&num_repeats] (std::error_code err, Clock::duration elap) {
        static int i = 0; std::cout << "World!  "; std::cout.flush();
        return (++i >= num_repeats ? false : true); };

    // call f every dur msec
    timer1.start_duration_timer(std::chrono::milliseconds(dur1), f1);
    timer2.start_duration_timer(std::chrono::milliseconds(dur2), f2);

    // release work_guard
    wg.reset();
    // reattach thread
    thr.join();
    
    std::cout << "\ntimer finished\n";

    return EXIT_SUCCESS;
}
