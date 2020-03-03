# Copyright 2018-2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# CMake 3.8 required for cxx_std_17 target_compile_features
# CMake 3.9 required for doxygen target

# Cliff note, Sep 5, 2018: I'm still a CMake newbie, so there's many improvements that can
# be made. I'm trying to use modern CMake techniques (see [References] (doc/references.md)).

cmake_minimum_required ( VERSION 3.9 )

option ( UTILITY_RACK_OPT_BUILD_TESTS  "Build and perform utility-rack tests" ON )
option ( UTILITY_RACK_OPT_BUILD_EXAMPLES  "Build perform utility-rack examples" ON )
option ( UTILITY_RACK_OPT_BUILD_DOCS  "Build doxygen documentation" OFF )

project ( utility-rack VERSION 1.0 LANGUAGES CXX )

set ( package_name "utility-rack" )

set ( include_source_dir "${CMAKE_SOURCE_DIR}/include" )
set ( third_party_include_source_dir "${CMAKE_SOURCE_DIR}/third_party" )

# Interface library:

add_library ( ${package_name} INTERFACE )

target_include_directories ( ${package_name} INTERFACE ${include_source_dir} )
target_include_directories ( ${package_name} INTERFACE ${third_party_include_source_dir} )
target_compile_features ( ${package_name} INTERFACE cxx_std_17)

if ( UTILITY_RACK_OPT_BUILD_TESTS )
  enable_testing()
  add_subdirectory ( test )
endif()

if ( UTILITY_RACK_OPT_BUILD_EXAMPLES )
  enable_testing()
  add_subdirectory ( example )
endif()

if ( UTILITY_RACK_OPT_BUILD_DOCS )
  add_subdirectory ( doc )
endif()

# end of file

