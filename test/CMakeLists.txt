# Copyright 2018-2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.8 )

project ( utility-rack-test VERSION 1.0 LANGUAGES CXX )

include ( "${cmake_all_repos_include_dir}/unit_test_vars.cmake" )

include ( "${cmake_include_dir}/header_dirs_var.cmake" )

set ( test_sources 
    "${test_source_dir}/queue/wait_queue_test.cpp"
    "${test_source_dir}/timer/periodic_timer_test.cpp"
    "${test_source_dir}/marshall/extract_append_test.cpp"
    #    "${test_source_dir}/marshall/marshall_test.cpp"
    "${test_source_dir}/marshall/shared_buffer_test.cpp"
    "${test_source_dir}/utility/cast_ptr_to_test.cpp"
    "${test_source_dir}/utility/erase_where_test.cpp"
    "${test_source_dir}/utility/make_byte_array_test.cpp"
    "${test_source_dir}/utility/overloaded_test.cpp"
    #    "${test_source_dir}/utility/forward_capture_test.cpp"
    "${test_source_dir}/utility/repeat_test.cpp" )

include ( "${cmake_include_dir}/add_target_dependencies.cmake" )

include ( "${cmake_all_repos_include_dir}/add_target_info_func.cmake" )
include ( "${cmake_all_repos_include_dir}/unit_test_main_lib.cmake" )
include ( "${cmake_all_repos_include_dir}/target_exe_func.cmake" )

enable_testing()

foreach ( test_src IN LISTS test_sources )
    get_filename_component ( targ ${test_src} NAME_WE )
    message ( "Calling unit_test_target_exe for: ${targ}" )
    unit_test_target_exe ( ${targ} ${test_src} )
endforeach()

# end of file

