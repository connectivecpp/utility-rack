# Copyright (c) 2024 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# I'm still learning CMake, so improvement suggestions are always welcome.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

project ( utility_rack 
	  LANGUAGES CXX 
	  DESCRIPTION "A collection of small utility classes and functions"
	  HOMEPAGE_URL "https://github.com/connectivecpp/utility-rack/" )

option ( UTILITY_RACK_BUILD_TESTS "Build unit tests" OFF )
option ( UTILITY_RACK_BUILD_EXAMPLES "Build examples" OFF )
option ( UTILITY_RACK_INSTALL "Install header only library" OFF )

# add library targets

add_library ( utility_rack INTERFACE )
add_library ( chops::utility_rack ALIAS utility_rack )

# configure library target

target_include_directories ( utility_rack INTERFACE
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
			     $<INSTALL_INTERFACE:include/> )
target_compile_features ( utility_rack INTERFACE cxx_std_20 )

# check to build unit tests
if ( ${UTILITY_RACK_BUILD_TESTS} )
  enable_testing()
  add_subdirectory ( test )
endif ()

# check to build example code
if ( ${UTILITY_RACK_BUILD_EXAMPLES} )
  add_subdirectory ( example )
endif ()

# check to install
if ( ${UTILITY_RACK_INSTALL} )
  set ( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt )
  include ( CPack )
endif ()

# end of file

