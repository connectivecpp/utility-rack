# Copyright 2018 by Cliff Green
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required (VERSION 3.5)
project (utility-rack VERSION 1.0 LANGUAGES CXX)

target_compile_features(utility-rack PUBLIC cxx_std_17)

include_directories("${PROJECT_BINARY_DIR}")
 
# add the executables
add_executable(WaitQueueTest wait_queue_test.cpp)
add_executable(RepeatTest repeat_test.cpp)
