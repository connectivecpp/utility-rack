# Copyright 2019-2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.12 )

project ( utility-rack-example VERSION 1.0 LANGUAGES CXX )

set ( example_source_dir "${CMAKE_SOURCE_DIR}/example" )

include ( "${cmake_include_dir}/header_dirs_var.cmake" )

set ( example_sources 
    "${example_source_dir}/repeat_demo.cpp"
    "${example_source_dir}/shared_buffer_demo.cpp"
    "${example_source_dir}/wait_queue_demo.cpp"
    "${example_source_dir}/threaded_queue_buffer_demo.cpp"
    "${example_source_dir}/timer_demo.cpp"
    "${example_source_dir}/erase_where_demo.cpp" )

include ( "${cmake_include_dir}/add_target_dependencies.cmake" )

include ( "${cmake_all_repos_include_dir}/add_target_info_func.cmake" )
include ( "${cmake_all_repos_include_dir}/target_exe_func.cmake" )

foreach ( example_src IN LISTS example_sources )
    get_filename_component ( targ ${example_src} NAME_WE )
    message ( "Calling target_exe for: ${targ}" )
    target_exe ( ${targ} ${example_src} )
endforeach()

# end of file

