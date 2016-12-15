//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel functional chaos result
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
#include "otgui/FunctionalChaosAnalysisResult.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FunctionalChaosAnalysisResult);

static Factory<FunctionalChaosAnalysisResult> RegisteredFactory;

FunctionalChaosAnalysisResult::FunctionalChaosAnalysisResult()
  : MetaModelAnalysisResult()
{

}


FunctionalChaosAnalysisResult* FunctionalChaosAnalysisResult::clone() const
{
  return new FunctionalChaosAnalysisResult(*this);
}


FunctionalChaosResult FunctionalChaosAnalysisResult::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
}


NumericalPoint FunctionalChaosAnalysisResult::getMean() const
{
  return mean_;
}


NumericalPoint FunctionalChaosAnalysisResult::getVariance() const
{
  return variance_;
}


SobolResult FunctionalChaosAnalysisResult::getSobolResult() const
{
  return sobolResult_;
}


/* String converter */
String FunctionalChaosAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mean=" << getMean()
      << " variance=" << getVariance()
      << " sobolResult=" << getSobolResult()
      << " errorQ2LOO=" << getErrorQ2LeaveOneOut()
      << " q2LOO=" << getQ2LeaveOneOut();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FunctionalChaosAnalysisResult::save(Advocate& adv) const
{
  MetaModelAnalysisResult::save(adv);
  adv.saveAttribute("functionalChaosResult_", functionalChaosResult_);
  adv.saveAttribute("mean_", mean_);
  adv.saveAttribute("variance_", variance_);
  adv.saveAttribute("sobolResult_", sobolResult_);
}


/* Method load() reloads the object from the StorageManager */
void FunctionalChaosAnalysisResult::load(Advocate& adv)
{
  MetaModelAnalysisResult::load(adv);
  adv.loadAttribute("functionalChaosResult_", functionalChaosResult_);
  adv.loadAttribute("mean_", mean_);
  adv.loadAttribute("variance_", variance_);
  adv.loadAttribute("sobolResult_", sobolResult_);
}
}