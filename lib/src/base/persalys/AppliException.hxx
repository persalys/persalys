//                                               -*- C++ -*-
/**
 *  @brief Class to define persalys Exceptions
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PERSALYS_EXCEPTION_HXX
#define PERSALYS_EXCEPTION_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Exception.hxx>

namespace PERSALYS
{

/*
 *
 * All exceptions defined for the user are listed below
 *
 */

#define NEW_PERSALYSEXCEPTION( CName ) class PERSALYS_BASE_API CName : public OT::Exception   \
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
NEW_PERSALYSEXCEPTION( PhysicalModelNotValidException );


/**
 * @class AnalysisExecutionFailedException
 * @brief Raised when analysis execution has failed
 * @internal
 */
NEW_PERSALYSEXCEPTION( AnalysisExecutionFailedException );


/**
 * @class InvalidValueException
 * @brief Raised when a value is not valid
 * @internal
 */
NEW_PERSALYSEXCEPTION( InvalidValueException );


/**
 * @class IOException
 * @brief Raised when using files
 * @internal
 */
NEW_PERSALYSEXCEPTION( IOException );

#undef NEW_PERSALYSEXCEPTION
}
#endif
