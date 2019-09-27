# AdaoCppLayer
#
# The module defines the following variables:
#  AdaoCppLayer_FOUND - the system has Adao Cpp
#  AdaoCppLayer_INCLUDE_DIR - where to find AdaoExchangeLayer.hxx
#  AdaoCppLayer_INCLUDE_DIRS - Adao Cpp includes
#  AdaoCppLayer_LIBRARY - where to find the Adao Cpp library
#  AdaoCppLayer_LIBRARIES - aditional libraries
#  AdaoCppLayer_ROOT_DIR - root dir (ex. /usr/local)

#=============================================================================
# Copyright 2010-2019, Julien Schueller
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met: 
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer. 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution. 
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies, 
# either expressed or implied, of the FreeBSD Project.
#=============================================================================


find_path (AdaoCppLayer_INCLUDE_DIR
  NAMES AdaoExchangeLayer.hxx
)
set (AdaoCppLayer_INCLUDE_DIRS ${AdaoCppLayer_INCLUDE_DIR})

# set (AdaoCppLayer_LIBRARIES ${AdaoCppLayer_LIBRARY})


# try to guess root dir from include dir
if (AdaoCppLayer_INCLUDE_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" AdaoCppLayer_ROOT_DIR ${AdaoCppLayer_INCLUDE_DIR})
# try to guess root dir from library dir
elseif (AdaoCppLayer_LIBRARY)
  string (REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" AdaoCppLayer_ROOT_DIR ${AdaoCppLayer_LIBRARY})
endif ()

if(AdaoCppLayer_ROOT_DIR)
  list(APPEND CMAKE_PREFIX_PATH "${AdaoCppLayer_ROOT_DIR}")
endif(AdaoCppLayer_ROOT_DIR)

find_library (AdaoCppLayer_LIBRARIES NAMES adaoexchange )

# handle the QUIETLY and REQUIRED arguments
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (AdaoCppLayer REQUIRED_VARS AdaoCppLayer_INCLUDE_DIR)

mark_as_advanced (
  AdaoCppLayer_INCLUDE_DIR
  AdaoCppLayer_INCLUDE_DIRS
  AdaoCppLayer_ROOT_DIR
)
