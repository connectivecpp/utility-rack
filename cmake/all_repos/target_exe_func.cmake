# Copyright 2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

function ( target_exe target src )
    add_executable        ( ${target} ${src} )
    add_target_info       ( ${target} )
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries ( ${target} PRIVATE Threads::Threads)
endfunction()

function ( unit_test_target_exe target src )
    target_exe ( ${target} ${src} )
    target_link_libraries ( ${target} PRIVATE ${main_test_lib_name} )
    add_test ( NAME ${target}${tester_suffix} COMMAND ${target} )
endfunction()

# end of file

