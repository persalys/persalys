//                                               -*- C++ -*-
/**
 *  @brief Class to define functional chaos
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/MetaModelAnalysis.hxx"

#include "otgui/MetaModel.hxx"
#include "otgui/DesignOfExperimentEvaluation.hxx"

#include <openturns/KPermutationsDistribution.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/Uniform.hxx>

using namespace OT;

namespace OTGUI
{

/* Default constructor */
MetaModelAnalysis::MetaModelAnalysis()
  : DesignOfExperimentAnalysis()
  , isDistributionComputed_(false)
  , analyticalValidation_(true)
  , testSampleValidation_(false)
  , kFoldValidation_(false)
  , leaveOneOutValidation_(false)
  , percentageTestSample_(20)
  , seedTestSample_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
  , nbFolds_(5)
  , seedKFold_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
MetaModelAnalysis::MetaModelAnalysis(const String& name, const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , isDistributionComputed_(false)
  , analyticalValidation_(true)
  , testSampleValidation_(false)
  , kFoldValidation_(false)
  , leaveOneOutValidation_(false)
  , percentageTestSample_(20)
  , seedTestSample_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
  , nbFolds_(5)
  , seedKFold_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
  if (designOfExperiment_.hasPhysicalModel() && designOfExperiment_.getPhysicalModel().hasStochasticInputs())
    isDeterministicAnalysis_ = false;
  if (designOfExperiment_.getOutputSample().getSize())
    setInterestVariables(designOfExperiment_.getOutputSample().getDescription());
}


/* Constructor with parameters */
MetaModelAnalysis::MetaModelAnalysis(const String& name, const Analysis& analysis)
  : DesignOfExperimentAnalysis(name)
  , isDistributionComputed_(false)
  , analyticalValidation_(true)
  , testSampleValidation_(false)
  , kFoldValidation_(false)
  , leaveOneOutValidation_(false)
  , percentageTestSample_(20)
  , seedTestSample_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
  , nbFolds_(5)
  , seedKFold_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
  DesignOfExperimentEvaluation * analysis_ptr = dynamic_cast<DesignOfExperimentEvaluation*>(analysis.getImplementation().get());

  if (!analysis_ptr)
  {
    throw InvalidArgumentException(HERE) << "The given analysis does not contain any design of experiments";
  }
  designOfExperiment_ = analysis_ptr->getResult().getDesignOfExperiment();
  if (designOfExperiment_.hasPhysicalModel() && designOfExperiment_.getPhysicalModel().hasStochasticInputs())
    isDeterministicAnalysis_ = false;
  setInterestVariables(analysis_ptr->getInterestVariables());
}


bool MetaModelAnalysis::analyticalValidation() const
{
  return analyticalValidation_;
}


void MetaModelAnalysis::setAnalyticalValidation(const bool validation)
{
  analyticalValidation_ = validation;
}


bool MetaModelAnalysis::testSampleValidation() const
{
  return testSampleValidation_;
}


void MetaModelAnalysis::setTestSampleValidation(const bool validation)
{
  testSampleValidation_ = validation;
}


bool MetaModelAnalysis::kFoldValidation() const
{
  return kFoldValidation_;
}


void MetaModelAnalysis::setKFoldValidation(const bool validation)
{
  kFoldValidation_ = validation;
}


bool MetaModelAnalysis::leaveOneOutValidation() const
{
  return leaveOneOutValidation_;
}


void MetaModelAnalysis::setLeaveOneOutValidation(const bool validation)
{
  leaveOneOutValidation_ = validation;
}


void MetaModelAnalysis::setTestSampleValidationPercentageOfPoints(const UnsignedInteger percentage)
{
  if (percentage == 0 || percentage >= 100)
    throw InvalidArgumentException(HERE) << "The percentage must be greater than 0 and lesser than 100";
  percentageTestSample_ = percentage;
}


void MetaModelAnalysis::setTestSampleValidationSeed(const UnsignedInteger seed)
{
  seedTestSample_ = seed;
}


PointWithDescription MetaModelAnalysis::getTestSampleValidationParameters() const
{
  PointWithDescription resu(2);
  resu[0] = percentageTestSample_;
  resu[1] = seedTestSample_;
  Description resuDescription(2);
  resuDescription[0] = "Percentage";
  resuDescription[1] = "Seed";
  resu.setDescription(resuDescription);
  return resu;
}


UnsignedInteger MetaModelAnalysis::getTestSampleValidationPercentageOfPoints() const
{
  return percentageTestSample_;
}


UnsignedInteger MetaModelAnalysis::getTestSampleValidationSeed() const
{
  return seedTestSample_;
}


void MetaModelAnalysis::setKFoldValidationNumberOfFolds(const UnsignedInteger nbFolds)
{
  if (nbFolds_ < 2)
    throw InvalidArgumentException(HERE) << "The number of folds must be greater than 1 ";
  nbFolds_ = nbFolds;
}


void MetaModelAnalysis::setKFoldValidationSeed(const UnsignedInteger seed)
{
  seedKFold_ = seed;
}


PointWithDescription MetaModelAnalysis::getKFoldValidationParameters() const
{
  PointWithDescription resu(2);
  resu[0] = nbFolds_;
  resu[1] = seedKFold_;
  Description resuDescription(2);
  resuDescription[0] = "Number of folds";
  resuDescription[1] = "Seed";
  resu.setDescription(resuDescription);
  return resu;
}


UnsignedInteger MetaModelAnalysis::getKFoldValidationNumberOfFolds() const
{
  return nbFolds_;
}


UnsignedInteger MetaModelAnalysis::getKFoldValidationSeed() const
{
  return seedKFold_;
}


Sample MetaModelAnalysis::getEffectiveInputSample() const
{
  // if data model
  if (!designOfExperiment_.hasPhysicalModel())
    return designOfExperiment_.getInputSample();

  // if only deterministic inputs
  if (!designOfExperiment_.getPhysicalModel().hasStochasticInputs() || !designOfExperiment_.getInputSample().getSize())
    return designOfExperiment_.getInputSample();

  // if the physical model has stochastic variables: we do not take into account the deterministic variables
  Indices inputIndices;
  for (UnsignedInteger i = 0; i < designOfExperiment_.getPhysicalModel().getInputDimension(); ++i)
    if (designOfExperiment_.getPhysicalModel().getInputs()[i].isStochastic())
      inputIndices.add(i);

  if (!inputIndices.check(designOfExperiment_.getInputSample().getDimension()))
    throw InvalidArgumentException(HERE) << "The design of experiments input sample dimension ("
                                         << designOfExperiment_.getInputSample().getDimension()
                                         << ") does not match the number of stochastic inputs in the physical model ("
                                         << inputIndices.getSize() << ")";

  return designOfExperiment_.getInputSample().getMarginal(inputIndices);
}


Sample MetaModelAnalysis::getEffectiveOutputSample() const
{
  if (!getInterestVariables().getSize())
    throw InvalidDimensionException(HERE) << "You have not defined output variable to be analysed. Set the list of interest variables.";

  const Description modelOutputsNames(designOfExperiment_.getOutputSample().getDescription());
  Indices outputIndices;
  for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
  {
    bool outputFound = false;
    for (UnsignedInteger j = 0; j < modelOutputsNames.getSize(); ++j)
    {
      if (modelOutputsNames[j] == getInterestVariables()[i])
      {
        outputIndices.add(j);
        outputFound = true;
        break;
      }
    }
    if (!outputFound)
      throw InvalidArgumentException(HERE) << "The output to analyze "  << getInterestVariables()[i]
                                           << " is not an output of the model " << modelOutputsNames
                                           << ". Check the list of interest variables.";
  }

  Sample effectiveOutputSample(designOfExperiment_.getOutputSample().getMarginal(outputIndices));
  effectiveOutputSample.setDescription(getInterestVariables());

  return effectiveOutputSample;
}


void MetaModelAnalysis::buildMetaModel(MetaModelAnalysisResult& result, const Function& function)
{
  MetaModel metaModel("MetaModel_", function);

  const Description inputsNames(function.getInputDescription());
  const Description outputsNames(function.getOutputDescription());

  // inputs
  for (UnsignedInteger i = 0; i < inputsNames.getSize(); ++i)
  {
    if (designOfExperiment_.getPhysicalModel().getInputNames().contains(inputsNames[i]))
      metaModel.addInput(designOfExperiment_.getPhysicalModel().getInputByName(inputsNames[i]));
    else
      metaModel.addInput(Input(inputsNames[i]));
  }

  // outputs
  for (UnsignedInteger i = 0; i < outputsNames.getSize(); ++i)
  {
    if (designOfExperiment_.getPhysicalModel().getOutputNames().contains(outputsNames[i]))
    {
      Output output(designOfExperiment_.getPhysicalModel().getOutputByName(outputsNames[i]));
      if (!output.getDescription().empty())
        output.setDescription(output.getDescription() + " - Meta model generated by " + getName());
      else
        output.setDescription("Meta model generated by " + getName());
      output.setValue(0.);
      output.setHasBeenComputed(false);
      metaModel.addOutput(output);
    }
    else
    {
      metaModel.addOutput(Output(outputsNames[i], getName() + " metamodel"));
    }
  }

  // copula
  if (designOfExperiment_.getPhysicalModel().hasStochasticInputs())
  {
    Collection<Copula> coll(designOfExperiment_.getPhysicalModel().getCopulaCollection());
    for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
    {
      metaModel.setCopula(coll[i].getDescription(), coll[i]);
    }
  }

  result.metaModel_ = metaModel;
}


void MetaModelAnalysis::computeError(const Sample& metaOutSample, const Sample& outSample, Point& residuals, Point& q2)
{
  // check
  if (!outSample.getSize())
    throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: outputSample is empty.\n";
  if (!metaOutSample.getSize())
    throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: metaOutSample is empty.\n";

  const UnsignedInteger size = outSample.getSize();
  const UnsignedInteger dimension = outSample.getDimension();

  // var Y
  const Point variance(outSample.computeVariance());

  residuals = Point(dimension);
  q2 = Point(dimension);

  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // sum[ (ŷ_j/j - y_j)^2 ]
    double quadraticResidual = 0.;
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      const double diff = metaOutSample(j, i) - outSample(j, i);
      quadraticResidual += diff * diff;
    }
    // sqrt ( sum[ (ŷ_j/j - y_j)^2 ] ) / n
    residuals[i] = sqrt(quadraticResidual) / size;
    // 1 - sum[ (ŷ_j/j - y_j)^2 ] / (n-1) / Var Y
    q2[i] = 1.0 - (quadraticResidual / (size - 1.0)) / variance[i];
  }
}


void MetaModelAnalysis::validateMetaModelResult(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  // check
  if (analyticalValidation_ || testSampleValidation_ || kFoldValidation_ || leaveOneOutValidation_)
  {
    if (!result.outputSample_.getSize())
      throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: The outputSample is empty.\n";
  }
  else // no validation requested
    return;

  // validation: Analytical
  if (analyticalValidation_)
  {
    try
    {
      computeAnalyticalValidation(result, inputSample);
    }
    catch (std::exception & ex)
    {
      warningMessage_ += OSS() << "Analytical validation failed: " << ex.what() << "\n";
    }
  }

  // validation: Test sample
  if (testSampleValidation_)
  {
    try
    {
      computeTestSampleValidation(result, inputSample);
    }
    catch (std::exception & ex)
    {
      warningMessage_ += OSS() << "Test sample validation failed: " << ex.what() << "\n";
    }
  }

  // validation: K-Fold
  if (kFoldValidation_)
  {
    try
    {
      computeKFoldValidation(result, inputSample);
    }
    catch (std::exception & ex)
    {
      warningMessage_ += OSS() << "K-Fold validation failed: " << ex.what() << "\n";
    }
  }

  // validation: Leave-one-out
  if (leaveOneOutValidation_)
  {
    try
    {
      computeLOOValidation(result, inputSample);
    }
    catch (std::exception & ex)
    {
      warningMessage_ += OSS() << "Leave-one-out validation failed: " << ex.what() << "\n";
    }
  }
}


void MetaModelAnalysis::computeAnalyticalValidation(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  throw NotYetImplementedException(HERE) << "Analytical validation not available";
}


void MetaModelAnalysis::computeTestSampleValidation(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  if (stopRequested_)
  {
    testSampleValidation_ = false;
    return;
  }
  informationMessage_ = "The Test sample validation is running.";
  notify("informationMessageUpdated");

  // dimensions
  const UnsignedInteger nbPoints = result.outputSample_.getSize();
  const UnsignedInteger testSampleSize = (nbPoints * percentageTestSample_ / 100 > 0 ? nbPoints * percentageTestSample_ / 100 : 1);

  // take randomly testSampleSize points in the DOE to use them for the validation
  RandomGenerator::SetSeed(seedTestSample_);
  const Point indicesTestSample(KPermutationsDistribution(testSampleSize, nbPoints).getRealization());

  // initialize samples
  Sample learningInSample(0, inputSample.getDimension());
  Sample learningOutSample(0, result.outputSample_.getDimension());
  Sample testInputSample(0, inputSample.getDimension());
  Sample testOutputSample(0, result.outputSample_.getDimension());

  for (UnsignedInteger i = 0; i < inputSample.getSize(); ++i)
  {
    if (!indicesTestSample.contains(i))
    {
      // points used to create the metamodel
      learningInSample.add(inputSample[i]);
      learningOutSample.add(result.outputSample_[i]);
    }
    else
    {
      // points used to validate the metamodel
      testInputSample.add(inputSample[i]);
      testOutputSample.add(result.outputSample_[i]);
    }
  }

  // build and run MetaModel Algorithm
  const Function function(runAlgo(learningInSample, learningOutSample));

  // fill result
  result.testSampleValidation_.metaModelSample_ = function(testInputSample);
  result.testSampleValidation_.parameters_ = getTestSampleValidationParameters();

  // compute Q2
  computeError(result.testSampleValidation_.metaModelSample_, testOutputSample, result.testSampleValidation_.residuals_, result.testSampleValidation_.q2_);
}


void MetaModelAnalysis::computeKFoldValidation(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  if (stopRequested_)
  {
    kFoldValidation_ = false;
    return;
  }
  informationMessage_ = "The K-Fold validation is running.";
  notify("informationMessageUpdated");

  // dimensions
  const UnsignedInteger nbPoints = inputSample.getSize();
  const UnsignedInteger outDim = result.outputSample_.getDimension();

  // initialize result
  Sample metaModelSample(nbPoints, outDim);
  Point residuals(outDim);
  Point q2(outDim);

  // we mix the samples points
  RandomGenerator::SetSeed(seedKFold_);
  const Point indices(KPermutationsDistribution(nbPoints, nbPoints).getRealization());
  Sample shuffledInSample(nbPoints, inputSample.getDimension());
  Sample shuffledOutSample(nbPoints, result.outputSample_.getDimension());
  for (UnsignedInteger i = 0; i < nbPoints; ++i)
  {
    shuffledInSample[i] = inputSample[indices[i]];
    shuffledOutSample[i] = result.outputSample_[indices[i]];
  }

  // for each fold:
  for (UnsignedInteger i = 0; i < nbFolds_; ++i)
  {
    if (stopRequested_)
    {
      kFoldValidation_ = false;
      return;
    }

    const int progressValue = (int) (i * 100 / nbFolds_);
    informationMessage_ = "The K-Fold validation is running.\nProgression: " + (OSS() << progressValue).str() + "%";
    notify("informationMessageUpdated");

    // get bounds of the fold_i
    const UnsignedInteger beginIndex = nbPoints / nbFolds_ * i;
    // if nbPoints%nbFolds != 0, we take the last values
    const UnsignedInteger endIndex = (i != nbFolds_ - 1 ? nbPoints / nbFolds_ * (i + 1) : nbPoints);

    // learning samples
    Sample learningInSample(shuffledInSample);
    learningInSample.erase(beginIndex, endIndex);
    Sample learningOutSample(shuffledOutSample);
    learningOutSample.erase(beginIndex, endIndex);

    // build and run MetaModel Algorithm
    const Function function(runAlgo(learningInSample, learningOutSample));

    // test samples
    const Sample testInputSample(shuffledInSample, beginIndex, endIndex);
    const Sample testOutputSample(shuffledOutSample, beginIndex, endIndex);

    // fill result
    const Sample tempMetaModelSample = function(testInputSample);
    for (UnsignedInteger j = 0; j < (endIndex - beginIndex); ++j)
    {
      metaModelSample[indices[beginIndex + j]] = tempMetaModelSample[j];
    }

    // compute Q2
    Point tempResiduals;
    Point tempQ2;
    computeError(tempMetaModelSample, testOutputSample, tempResiduals, tempQ2);
    residuals += tempResiduals;
    q2 += tempQ2;
  }

  // fill result
  result.kFoldValidation_.metaModelSample_ = metaModelSample;
  // compute the mean of the q2
  result.kFoldValidation_.residuals_ = residuals / nbFolds_;
  result.kFoldValidation_.q2_ = q2 / nbFolds_;

  result.kFoldValidation_.parameters_ = getKFoldValidationParameters();
}


void MetaModelAnalysis::computeLOOValidation(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  informationMessage_ = "The Leave-one-out validation is running.";
  notify("informationMessageUpdated");

  // initialize result
  Sample metaModelSample(inputSample.getSize(), result.outputSample_.getDimension());

  for (UnsignedInteger i = 0; i < inputSample.getSize(); ++i)
  {
    if (stopRequested_)
    {
      leaveOneOutValidation_ = false;
      return;
    }

    const int progressValue = (int) (i * 100 / inputSample.getSize());
    informationMessage_ = "The Leave-one-out validation is running.\nProgression: " + (OSS() << progressValue).str() + "%";
    notify("informationMessageUpdated");

    // learning samples : remove input_i
    Sample learningInSample(inputSample);
    learningInSample.erase(i);
    Sample learningOutSample(result.outputSample_);
    learningOutSample.erase(i);

    // build and run MetaModel Algorithm
    const Function function(runAlgo(learningInSample, learningOutSample));

    // evaluate output at input_i
    metaModelSample[i] = function(inputSample[i]);
  }
  // fill result
  result.looValidation_.metaModelSample_ = metaModelSample;

  // compute Q2
  computeError(result.looValidation_.metaModelSample_, result.outputSample_, result.looValidation_.residuals_, result.looValidation_.q2_);
}


/* String converter */
String MetaModelAnalysis::__repr__() const
{
  OSS oss;
  oss << DesignOfExperimentAnalysis::__repr__()
      << " analyticalValidation=" << analyticalValidation()
      << " testSampleValidation=" << testSampleValidation()
      << " kFoldValidation=" << kFoldValidation()
      << " leaveOneOutValidation=" << leaveOneOutValidation()
      << " test sample parameters=" << getTestSampleValidationParameters()
      << " k-Fold parameters=" << getKFoldValidationParameters();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MetaModelAnalysis::save(Advocate& adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("analyticalValidation_", analyticalValidation_);
  adv.saveAttribute("testSampleValidation_", testSampleValidation_);
  adv.saveAttribute("kFoldValidation_", kFoldValidation_);
  adv.saveAttribute("leaveOneOutValidation_", leaveOneOutValidation_);
  adv.saveAttribute("percentageTestSample_", percentageTestSample_);
  adv.saveAttribute("seedTestSample_", seedTestSample_);
  adv.saveAttribute("nbFolds_", nbFolds_);
  adv.saveAttribute("seedKFold_", seedKFold_);
}


/* Method load() reloads the object from the StorageManager */
void MetaModelAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("analyticalValidation_", analyticalValidation_);
  adv.loadAttribute("testSampleValidation_", testSampleValidation_);
  adv.loadAttribute("kFoldValidation_", kFoldValidation_);
  adv.loadAttribute("leaveOneOutValidation_", leaveOneOutValidation_);
  adv.loadAttribute("percentageTestSample_", percentageTestSample_);
  adv.loadAttribute("seedTestSample_", seedTestSample_);
  adv.loadAttribute("nbFolds_", nbFolds_);
  adv.loadAttribute("seedKFold_", seedKFold_);
}
}
