//                                               -*- C++ -*-
/**
 *  @brief Class to define otgui Exceptions
 *
 *  Copyright 2015-2018 EDF-Phimeca
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

#ifndef OTGUI_EXCEPTION_HXX
#define OTGUI_EXCEPTION_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/Exception.hxx>

namespace OTGUI
{

/*
 *
 * All exceptions defined for the user are listed below
 *
 */

#define NEW_OTGUIEXCEPTION( CName ) class OTGUI_API CName : public OT::Exception   \
  {                                                             \
  public:                                                       \
    CName (const OT::PointInSourceFile & point);                    \
    virtual ~CName () throw();                                  \
    template <class T> CName & operator << (T obj)              \
    {                                                           \
      this->Exception::operator << ( obj );                     \
      return *this;                                             \
    }                                                           \
  }

/**
 * @class PhysicalModelNotValidException
 * @brief Raised when physical model is not valid
 * @internal
 */
NEW_OTGUIEXCEPTION( PhysicalModelNotValidException );


/**
 * @class AnalysisExecutionFailedException
 * @brief Raised when analysis execution has failed
 * @internal
 */
NEW_OTGUIEXCEPTION( AnalysisExecutionFailedException );


/**
 * @class InvalidValueException
 * @brief Raised when a value is not valid
 * @internal
 */
NEW_OTGUIEXCEPTION( InvalidValueException );


/**
 * @class IOException
 * @brief Raised when using files
 * @internal
 */
NEW_OTGUIEXCEPTION( IOException );

#undef NEW_OTGUIEXCEPTION
}
#endif
