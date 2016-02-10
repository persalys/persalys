//                                               -*- C++ -*-
/**
 *  @brief Results of the evaluation of the physical models 
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/ModelEvaluationResult.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluationResult);

ModelEvaluationResult::ModelEvaluationResult()
{

}


ModelEvaluationResult::ModelEvaluationResult(NumericalSample inputSample, NumericalSample outputSample)
  : inputSample_(inputSample)
  , outputSample_(outputSample)
{
}


ModelEvaluationResult* ModelEvaluationResult::clone() const
{
  return new ModelEvaluationResult(*this);
}


NumericalSample ModelEvaluationResult::getInputSample() const
{
  return inputSample_;
}


NumericalSample ModelEvaluationResult::getOutputSample() const
{
  return outputSample_;
}
}