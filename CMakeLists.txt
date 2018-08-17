# Copyright 2018 by Cliff Green
#
# https://github.com/cliffg-softwarelibre/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# CMake 3.8 required for cxx_std_17 target_compile_features

cmake_minimum_required ( VERSION 3.8 )

option ( UTILITY_RACK_OPT_BUILD_TESTS  "Build and perform utility-rack tests" ON )

project ( utility-rack VERSION 1.0 LANGUAGES CXX )

set ( package_name "utility-rack" )

set ( include_source_dir "${PROJECT_SOURCE_DIR}/include" )

# Interface library:

add_library ( ${package_name} INTERFACE )

target_include_directories ( ${package_name} INTERFACE "$<BUILD_INTERFACE:${include_source_dir}>" )

target_compile_features ( ${package_name} PUBLIC cxx_std_17)

enable_testing()

if ( UTILITY_RACK_OPT_BUILD_TESTS )
  add_subdirectory ( test )
endif()

# end of file
