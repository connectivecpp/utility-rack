# Copyright (c) 2024-2025 by Cliff Green
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

# create project
project ( utility_rack_example LANGUAGES CXX )

set ( example_app_names  erase_where_example
                         repeat_example )

# add executable
foreach ( example_app_name IN LISTS example_app_names )
  message ( "Creating example executable: ${example_app_name}" )
  add_executable ( ${example_app_name} ${example_app_name}.cpp )
  target_compile_features ( ${example_app_name} PRIVATE cxx_std_20 )
  target_link_libraries ( ${example_app_name} PRIVATE utility_rack )
endforeach()

