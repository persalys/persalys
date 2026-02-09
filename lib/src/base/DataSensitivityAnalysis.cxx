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
#include <openturns/CorrelationAnalysis.hxx>
#include <openturns/BootstrapExperiment.hxx>

using namespace OT;

namespace PERSALYS
{
CLASSNAMEINIT(DataSensitivityAnalysis)

const static Factory<DataSensitivityAnalysis> Factory_DataSensitivityAnalysis;

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
  return DataSensitivityAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
}

bool DataSensitivityAnalysis::hasValidResult() const
{
  return !result_.getFirstOrderSobolIndices().isEmpty() && result_.getFirstOrderSobolIndicesInterval().getSize() == result_.getFirstOrderSobolIndices().getSize();
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
  computeSobolIndices();
  computeSRCIndices();
  checkIndependance();
}

void DataSensitivityAnalysis::computeSobolIndices()
{
  const Sample outSample{designOfExperiment_.getOutputSample()};
  Sample marginalOutSample;
  const Sample inSample{designOfExperiment_.getInputSample()};
  const auto nbOutputs = outSample.getDimension();

  for(UnsignedInteger i = 0 ; i < nbOutputs; ++i)
  {
    marginalOutSample = outSample.getMarginal(i);
    auto algo = RankSobolSensitivityAlgorithm(inSample, marginalOutSample);  // change to setDesign when OT has been fixed (OT > 1.25)
    result_.firstOrderSobolIndices_.add(algo.getFirstOrderIndices());
    result_.firstOrderSobolIndicesInterval_.add(algo.getFirstOrderIndicesInterval());
  }
}

/* adapted from SRCAnalysis::launch */
void DataSensitivityAnalysis::computeSRCIndices()
{
  const Sample inputSample{designOfExperiment_.getInputSample()};
  const Sample outputSample{designOfExperiment_.getOutputSample()};

  const UnsignedInteger nbOutputs = outputSample.getDimension();
  const UnsignedInteger nbInputs  = inputSample.getDimension();

  Sample indices{0, inputSample.getDimension()};
  Sample signedIndices{0, inputSample.getDimension()};
  Point r2(nbOutputs);

  for(UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
  {
    const Point signedSRC{CorrelationAnalysis{inputSample, outputSample.getMarginal(i)}.computeSRC()};
    signedIndices.add(signedSRC);
    Point unscaledSRC;
    for(UnsignedInteger j = 0 ; j < nbInputs ; ++j)
    {
      const Scalar squaredSRC = signedSRC[j] * signedSRC[j];
      unscaledSRC.add(squaredSRC);
      r2[i] += squaredSRC;
    }
    indices.add(unscaledSRC);
  }

  // set results
  indices.setDescription(inputSample.getDescription());
  result_.SRCIndices_       = indices;
  result_.signedSRCIndices_ = signedIndices;
  result_.r2_               = r2;

  // Compute bootstrap confidence intervals
  const UnsignedInteger bootstrapSize = ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize");
  const Scalar alpha = ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel");
  Indices inIndices(nbInputs);
  inIndices.fill();

  // - get bootstrap experiment
  Sample sample{inputSample};
  sample.stack(outputSample);
  BootstrapExperiment bootstrapExp{sample};
  // - compute signed SRC
  Collection signedSRCBootstrap(nbOutputs, Sample{0, nbInputs});
  Collection unscaledSRCBootstrap(nbOutputs, Sample{0, nbInputs});
  for (UnsignedInteger i = 0; i < bootstrapSize; ++i)
  {
    const Sample bootstrapSample{bootstrapExp.generate()};
    for (UnsignedInteger j = 0; j < nbOutputs; ++j)
    {
      const Point signedSRC{CorrelationAnalysis{bootstrapSample.getMarginal(inIndices), bootstrapSample.getMarginal(nbInputs + j)}.computeSRC()};
      signedSRCBootstrap[j].add(signedSRC);

      Point unscaledSRC;
      for (UnsignedInteger k = 0; k < nbInputs; ++k)
        unscaledSRC.add(signedSRC[k] * signedSRC[k]);
      unscaledSRCBootstrap[j].add(unscaledSRC);
    }
  }
  // - compute bounds
  PersistentCollection<Interval> signedSRCInterval{nbOutputs};
  PersistentCollection<Interval> unscaledSRCInterval{nbOutputs};
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    Point upperBound{signedSRCBootstrap[i].computeQuantilePerComponent(alpha)};
    Point lowerBound{signedSRCBootstrap[i].computeQuantilePerComponent(1 - alpha)};
    signedSRCInterval[i] = Interval(lowerBound, upperBound);

    Point upperBound2{unscaledSRCBootstrap[i].computeQuantilePerComponent(alpha)};
    Point lowerBound2{unscaledSRCBootstrap[i].computeQuantilePerComponent(1 - alpha)};
    unscaledSRCInterval[i] = Interval(lowerBound2, upperBound2);
  }
  result_.SRCIndicesInterval_ = unscaledSRCInterval;
  result_.signedSRCIndicesInterval_ = signedSRCInterval;
}

void DataSensitivityAnalysis::checkIndependance()
{
  const Sample inSample{designOfExperiment_.getInputSample()};
  const Description inputNames{inSample.getDescription()};
  const auto nbInputs = inSample.getDimension();

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
        warningMessage << "Variables " << inputNames[i] << " and " << inputNames[j] << " are not independent (Spearman test failed with p-value " << testResult.getPValue() << ").\n";
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

bool DataSensitivityAnalysis::CanBeLaunched(String &errorMessage, const DesignOfExperiment &doe)
{
  if (!DesignOfExperimentAnalysis::CanBeLaunched(errorMessage, doe))
    return false;
  // doe must have in/output data
  if (!doe.getOutputSample().getSize() || !doe.getInputSample().getSize())
    errorMessage = "The sample must contain data for input and output variables.";
  if (doe.getInputSample().getDimension() < 2)
    errorMessage = "The sample must contain at least two input variables.";
  return errorMessage.empty();
}

} // namespace PERSALYS