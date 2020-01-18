# Copyright 2018-2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.9 )

project ( utility-rack-doc VERSION 1.0 LANGUAGES CXX )

find_package ( Doxygen )

if (DOXYGEN_FOUND)
  doxygen_add_docs ( docs
                     include test example 
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                     COMMENT "Document generation command"
                   )

else (DOXYGEN_FOUND)
  message("Doxygen needs to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)



# end of file

