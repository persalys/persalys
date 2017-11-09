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

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(MetaModelAnalysisResult)

static Factory<MetaModelAnalysisResult> Factory_MetaModelAnalysisResult;

MetaModelAnalysisResult::MetaModelAnalysisResult()
  : PersistentObject()
  , metaModel_()
  , analyticalValidation_("Analytical")
  , testSampleValidation_("Test sample")
  , kFoldValidation_("K-Fold")
  , looValidation_("Leave-one-out")
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


Sample MetaModelAnalysisResult::getOutputSample() const
{
  return outputSample_;
}


Sample MetaModelAnalysisResult::getMetaModelOutputSample() const
{
  return metaModelOutputSample_;
}


MetaModelValidationResult MetaModelAnalysisResult::getAnalyticalValidation() const
{
  return analyticalValidation_;
};


MetaModelValidationResult MetaModelAnalysisResult::getTestSampleValidation() const
{
  return testSampleValidation_;
};


MetaModelValidationResult MetaModelAnalysisResult::getKFoldValidation() const
{
  return kFoldValidation_;
};


MetaModelValidationResult MetaModelAnalysisResult::getLeaveOneOutValidation() const
{
  return looValidation_;
};


std::vector<MetaModelValidationResult> MetaModelAnalysisResult::getValidations() const
{
  std::vector<MetaModelValidationResult> validations;
  if (analyticalValidation_.getQ2().getSize())
    validations.push_back(analyticalValidation_);
  if (testSampleValidation_.getQ2().getSize())
    validations.push_back(testSampleValidation_);
  if (kFoldValidation_.getQ2().getSize())
    validations.push_back(kFoldValidation_);
  if (looValidation_.getQ2().getSize())
    validations.push_back(looValidation_);
  return validations;
}


/* String converter */
String MetaModelAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " metaModel_=" << getMetaModel();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MetaModelAnalysisResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("metaModel_", metaModel_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("metaModelOutputSample_", metaModelOutputSample_);
  adv.saveAttribute("analyticalValidation_", analyticalValidation_);
  adv.saveAttribute("testSampleValidation_", testSampleValidation_);
  adv.saveAttribute("kFoldValidation_", kFoldValidation_);
  adv.saveAttribute("looValidation_", looValidation_);
}


/* Method load() reloads the object from the StorageManager */
void MetaModelAnalysisResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("metaModel_", metaModel_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("metaModelOutputSample_", metaModelOutputSample_);
  adv.loadAttribute("analyticalValidation_", analyticalValidation_);
  adv.loadAttribute("testSampleValidation_", testSampleValidation_);
  adv.loadAttribute("kFoldValidation_", kFoldValidation_);
  adv.loadAttribute("looValidation_", looValidation_);
}
}
