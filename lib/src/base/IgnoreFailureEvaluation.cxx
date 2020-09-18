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

#include "persalys/IgnoreFailureEvaluation.hxx"

namespace PERSALYS
{

  CLASSNAMEINIT(IgnoreFailureEvaluation)

  /* Default constructor */
  IgnoreFailureEvaluation::IgnoreFailureEvaluation()
    : EvaluationImplementation() {}

  void IgnoreFailureEvaluation::setIgnoreFailure(const bool & ignore) {
    ignoreFailure_ = ignore;
  }

}
