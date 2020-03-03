# Copyright 2020 by Cliff Green
#
# https://github.com/connectivecpp/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_package ( Doxygen 
               OPTIONAL_COMPONENTS dot mscgen dia
             )

if (DOXYGEN_FOUND)
  set ( DOXYGEN_PROJECT_NAME ${doxygen_proj_name} )
  set ( DOXYGEN_EXCLUDE_PATTERNS */detail/* )
  doxygen_add_docs ( docs
                     include
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                     COMMENT "Document generation command"
                   )

else (DOXYGEN_FOUND)
  message("Doxygen needs to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

# end of file

