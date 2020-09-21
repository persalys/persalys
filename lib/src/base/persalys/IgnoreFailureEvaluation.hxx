//                                               -*- C++ -*-
/**
 *  @brief IgnoreFailureEvaluation implements base evaluation for persalys models
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_EVALUATION_HXX
#define PERSALYS_EVALUATION_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/EvaluationImplementation.hxx>

namespace PERSALYS
{
/**
 * @class IgnoreFailureEvaluation
 *
 * The class that implements the evaluation base class.
 */
class PERSALYS_BASE_API IgnoreFailureEvaluation : public OT::EvaluationImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  IgnoreFailureEvaluation();

  virtual void setIgnoreFailure(const bool & ignore);

protected:
  bool ignoreFailure_ = false;
};
}
#endif
