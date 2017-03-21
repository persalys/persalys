//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel result
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/MetaModelAnalysisResult.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MetaModelAnalysisResult);

static Factory<MetaModelAnalysisResult> RegisteredFactory;

MetaModelAnalysisResult::MetaModelAnalysisResult()
  : PersistentObject()
  , metaModel_()
{

}


MetaModelAnalysisResult* MetaModelAnalysisResult::clone() const
{
  return new MetaModelAnalysisResult(*this);
}


PhysicalModel MetaModelAnalysisResult::getMetaModel() const
{
  return metaModel_;
}


NumericalSample MetaModelAnalysisResult::getOutputSample() const
{
  return outputSample_;
}


NumericalSample MetaModelAnalysisResult::getMetaModelOutputSample() const
{
  return metaModelOutputSample_;
}


NumericalSample MetaModelAnalysisResult::getMetaModelOutputSampleLeaveOneOut() const
{
  return metaModelOutputSampleLOO_;
}


NumericalPoint MetaModelAnalysisResult::getErrorQ2LeaveOneOut() const
{
  return errorQ2LOO_;
}


NumericalPoint MetaModelAnalysisResult::getQ2LeaveOneOut() const
{
  return q2LOO_;
}


/* String converter */
String MetaModelAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " metaModel_=" << getMetaModel()
      << " errorQ2LOO=" << getErrorQ2LeaveOneOut()
      << " q2LOO=" << getQ2LeaveOneOut();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MetaModelAnalysisResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("metaModel_", metaModel_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("metaModelOutputSample_", metaModelOutputSample_);
  adv.saveAttribute("metaModelOutputSampleLOO_", metaModelOutputSampleLOO_);
  adv.saveAttribute("errorQ2LOO_", errorQ2LOO_);
  adv.saveAttribute("q2LOO_", q2LOO_);
}


/* Method load() reloads the object from the StorageManager */
void MetaModelAnalysisResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("metaModel_", metaModel_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("metaModelOutputSample_", metaModelOutputSample_);
  adv.loadAttribute("metaModelOutputSampleLOO_", metaModelOutputSampleLOO_);
  adv.loadAttribute("errorQ2LOO_", errorQ2LOO_);
  adv.loadAttribute("q2LOO_", q2LOO_);
}
}