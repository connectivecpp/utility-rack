# Copyright 2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Note - pthread library link will soon be cleaned up

function ( make_exe target src )
    add_executable        ( ${target} ${src} )
    add_target_info       ( ${target} )
    target_link_libraries ( ${target} PRIVATE pthread )
    message ( "Executable to create: ${target}" )
endfunction()

function ( make_unit_test_exe target src )
    add_executable        ( ${target} ${src} )
    add_target_info       ( ${target} )
    target_link_libraries ( ${target} PRIVATE pthread )
    target_link_libraries ( ${target} PRIVATE ${main_test_lib_name} )
    message ( "Unit test executable to create: ${target}" )
    add_test ( NAME ${target}${tester_suffix} COMMAND ${target} )
endfunction()

# end of file

