# Copyright 2018 by Cliff Green
#
# https://github.com/cliffg-softwarelibre/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.8 )

project ( test LANGUAGES CXX )

target_compile_features ( test PUBLIC cxx_std_17 )

set ( MAIN_SRC main_test.cpp )
set ( TEST_SOURCES 
    queue/wait_queue_test.cpp
    timer/periodic_timer_test.cpp
    utility/erase_where_test.cpp
    utility/make_byte_array_test.cpp
    utility/repeat_test.cpp
    utility/shared_buffer_test.cpp )

set ( TEST_TARGETS 
    wait_queue_test
    periodic_timer_test
    erase_where_test
    make_byte_array_test
    repeat_test
    shared_buffer_test )

set ( HDR_DIR
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/Catch2/include
    ${PROJECT_SOURCE_DIR}/networking_ts_impl/include
    ${PROJECT_SOURCE_DIR}/ring-span-lite/include )

    
set ( OPTIONS "" )
set ( DEFINITIONS "" )

function ( make_target target src )
    add_executable            ( ${target} ${MAIN_SRC} ${SRC} )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_include_directories( ${target} PRIVATE ${HDRDIR} )
endfunction()

foreach (  )

enable_testing()



