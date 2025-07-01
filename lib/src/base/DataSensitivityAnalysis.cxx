//                                               -*- C++ -*-
/**
 *  @brief Base class to make a sensitivity analysis of a data model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

#include "persalys/DataSensitivityAnalysis.hxx"
#include <openturns/RankSobolSensitivityAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/HypothesisTest.hxx>

using namespace OT;

namespace PERSALYS
{
CLASSNAMEINIT(DataSensitivityAnalysis)

static Factory<DataSensitivityAnalysis> Factory_DataSensitivityAnalysis;

DataSensitivityAnalysis::DataSensitivityAnalysis()
  : DesignOfExperimentAnalysis()
{
}

DataSensitivityAnalysis::DataSensitivityAnalysis(const String &name, const DesignOfExperiment & design)
  : DesignOfExperimentAnalysis(name, design)
{
}

DataSensitivityAnalysis * DataSensitivityAnalysis::clone() const
{
  return new DataSensitivityAnalysis(*this);
}

bool DataSensitivityAnalysis::canBeLaunched(String &errorMessage) const
{
  if (const bool canBeLaunched = DesignOfExperimentAnalysis::canBeLaunched(errorMessage); !canBeLaunched)
    return false;
  // doe must have in/output data
  if (!getDesignOfExperiment().getOutputSample().getSize() || !getDesignOfExperiment().getInputSample().getSize())
    errorMessage = "The design of experiments must contain data for input and output variables.";
  return errorMessage.empty();
}

bool DataSensitivityAnalysis::hasValidResult() const
{
  return !result_.getFirstOrderIndices().isEmpty() && result_.getFirstOrderIndicesInterval().getSize() == result_.getFirstOrderIndices().getSize();
}

const DataSensitivityAnalysisResult & DataSensitivityAnalysis::getResult() const
{
  return result_;
}

void DataSensitivityAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = DataSensitivityAnalysisResult(designOfExperiment_);

}

void DataSensitivityAnalysis::launch()
{
  const Sample outSample = designOfExperiment_.getOutputSample();
  Sample marginalOutSample;
  const Sample inSample = designOfExperiment_.getInputSample();
  const Description inputNames = inSample.getDescription();
  const auto nbInputs = inSample.getDimension();
  const auto nbOutputs = outSample.getDimension();

  for(UnsignedInteger i = 0 ; i < nbOutputs; ++i)
  {
    marginalOutSample = outSample.getMarginal(i);
    auto algo = RankSobolSensitivityAlgorithm(inSample, marginalOutSample);  // change to setDesign when OT has been fixed (OT > 1.25)
    result_.firstOrderIndices_.add(algo.getFirstOrderIndices());
    result_.firstOrderIndicesInterval_.add(algo.getFirstOrderIndicesInterval());
  }

  /*independence check*/
  OSS warningMessage;
  warningMessage << "Warning: ";
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    for (UnsignedInteger j = i + 1; j < nbInputs; ++j)
    {
      const TestResult testResult = HypothesisTest::Spearman(inSample.getMarginal(i), inSample.getMarginal(j));
      const bool isIndependent = testResult.getBinaryQualityMeasure();
      if (!isIndependent)
      {
        warningMessage << "Variables " << inputNames[i] << " and " << inputNames[j] << " are not independent (Spearman test failed with p-value " << testResult.getPValue() << "). ";
      }
      result_.isIndependent_ = result_.isIndependent_ && isIndependent;
    }
  }
  result_.independenceWarningMessage_ = warningMessage.str();
  if (result_.isIndependent_)
  {
    result_.independenceWarningMessage_ = "";
  }
}

String DataSensitivityAnalysis::__repr__() const
{
  return OSS() << "class=" << getClassName()
               << " name=" << getName()
               << " designOfExperiment=" << designOfExperiment_;
}

void DataSensitivityAnalysis::save(OT::Advocate & adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("result_", result_);
}

void DataSensitivityAnalysis::load(OT::Advocate & adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("result_", result_); 
}

} // namespace PERSALYS