//                                               -*- C++ -*-
/**
 *  @brief Class to define otgui Exceptions 
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/OTguiException.hxx"

using namespace OT;

namespace OTGUI
{

#define DEFINE_OTGUIEXCEPTION( CName )               \
  static const char * CName ## Name = #CName;   \
  static const CName CName ## Obj ( HERE );     \
  CName::CName(const PointInSourceFile & point) \
    : Exception(point, CName ## Name) {}        \
  CName::~CName () throw() {}

DEFINE_OTGUIEXCEPTION( PhysicalModelNotValidException )
DEFINE_OTGUIEXCEPTION( AnalysisExecutionFailedException )
DEFINE_OTGUIEXCEPTION( InvalidValueException )

#undef DEFINE_OTGUIEXCEPTION
}