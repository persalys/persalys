#                                               -*- cmake -*-
#
#  UsePersalys.cmake
#
#  Use Persalys in CMake files
#
#  Copyright 2015-2020 EDF-Phimeca
#
#  This library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#  @author dutka
#  @date   2010-02-04 16:44:49 +0100 (Thu, 04 Feb 2010)
#

add_definitions     ( ${PERSALYS_DEFINITIONS} )
include_directories ( ${PERSALYS_INCLUDE_DIRS} )
link_directories    ( ${PERSALYS_LIBRARY_DIRS} )

