//                                               -*- C++ -*-
/**
 *  @brief Results of a simulation analysis
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
#include "persalys/EvaluationResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(EvaluationResult)

static Factory<EvaluationResult> Factory_EvaluationResult;

/* Default constructor */
EvaluationResult::EvaluationResult()
  : AnalysisResult()
  , designOfExperiment_()
{

}


/* Constructor with parameters */
EvaluationResult::EvaluationResult(const DesignOfExperiment& design)
  : AnalysisResult()
  , designOfExperiment_(design)
{

}


EvaluationResult* EvaluationResult::clone() const
{
  return new EvaluationResult(*this);
}


DesignOfExperiment EvaluationResult::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


/* String converter */
String EvaluationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void EvaluationResult::save(Advocate & adv) const
{
  AnalysisResult::save(adv);
  adv.saveAttribute("designOfExperiment_", designOfExperiment_);
}


/* Method load() reloads the object from the StorageManager */
void EvaluationResult::load(Advocate & adv)
{
  AnalysisResult::load(adv);
  adv.loadAttribute("designOfExperiment_", designOfExperiment_);
}
}
