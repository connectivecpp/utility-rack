# Copyright (c) 2024 by Cliff Green
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

# create project
project ( utility_test LANGUAGES CXX )

# add dependencies
include ( ../cmake/download_cpm.cmake )
CPMAddPackage ( "gh:catchorg/Catch2@3.7.0" )

set ( test_app_names  cast_ptr_to_test
                      erase_where_test
		      #                      forward_capture_test
                      byte_array_test
                      overloaded_test
                      repeat_test )

# add executable
foreach ( test_app_name IN LISTS test_app_names )
  message ( "Creating test executable: ${test_app_name}" )
  add_executable ( ${test_app_name} ${test_app_name}.cpp )
  target_compile_features ( ${test_app_name} PRIVATE cxx_std_20 )
  target_link_libraries ( ${test_app_name} PRIVATE utility_rack Catch2::Catch2WithMain )
endforeach()

enable_testing()

foreach ( test_app_name IN LISTS test_app_names )
  message ( "Creating test: run_${test_app_name}" )
  add_test ( NAME run_${test_app_name} COMMAND ${test_app_name} )
  set_tests_properties ( run_${test_app_name} 
    PROPERTIES PASS_REGULAR_EXPRESSION "All tests passed"
  )
endforeach()

