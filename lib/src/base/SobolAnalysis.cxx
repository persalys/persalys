//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
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
#include "otgui/SobolAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/SaltelliSensitivityAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/ResourceMap.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(SobolAnalysis)

static Factory<SobolAnalysis> Factory_SobolAnalysis;

/* Default constructor */
SobolAnalysis::SobolAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , bootstrapConfidenceLevel_(ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , result_()
{
}


/* Constructor with parameters */
SobolAnalysis::SobolAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , bootstrapConfidenceLevel_(ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , result_()
{
}


/* Virtual constructor */
SobolAnalysis* SobolAnalysis::clone() const
{
  return new SobolAnalysis(*this);
}


UnsignedInteger SobolAnalysis::getBootstrapSize() const
{
  return bootstrapSize_;
}


void SobolAnalysis::setBootstrapSize(const UnsignedInteger size)
{
  if (size < 1)
    throw InvalidArgumentException(HERE) << "Bootstrap sampling size should be positive. Here, bootstrapSize=" << size;
  bootstrapSize_ = size;
}


double SobolAnalysis::getBootstrapConfidenceLevel() const
{
  return bootstrapConfidenceLevel_;
}


void SobolAnalysis::setBootstrapConfidenceLevel(const double level)
{
  if (!(level >= 0.0) || !(level < 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level value should be in ]0,1[. Here, confidence level=" << level;
  bootstrapConfidenceLevel_ = level;
}


void SobolAnalysis::run()
{
  isRunning_ = true;
  try
  {
    // clear result
    initialize();
    result_ = SobolResult();

    // check
    if (getMaximumCalls() < (getBlockSize() * (getPhysicalModel().getStochasticInputNames().getSize() + 2)))
      throw InvalidValueException(HERE) << "The block size (" << getBlockSize()
                                        << ") can not be superior to: max_calls (" << getMaximumCalls()
                                        << ") / (number_of_inputs (" << getPhysicalModel().getStochasticInputNames().getSize()
                                        << ") + 2)="
                                        << (getMaximumCalls() / (getPhysicalModel().getStochasticInputNames().getSize() + 2));

    // initialization
    RandomGenerator::SetSeed(getSeed());

    const UnsignedInteger nbInputs(getPhysicalModel().getStochasticInputNames().getSize());
    const UnsignedInteger nbOutputs(getInterestVariables().getSize());

    const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
    const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / (getBlockSize() * (2 + nbInputs)))) : (UnsignedInteger)std::numeric_limits<int>::max();
    const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getMaximumCalls() % (getBlockSize() * (2 + nbInputs)) : 0;
    const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : (modulo / (2 + nbInputs));

    Scalar coefficientOfVariation = -1.0;
    clock_t elapsedTime = 0;
    const clock_t startTime = clock();
    UnsignedInteger outerSampling = 0;

    Sample inputDesign;
    Sample outputDesign;

    Sample X1(0, nbInputs);
    Sample X2(0, nbInputs);
    Collection<Sample> crossX1(nbInputs, Sample(0, nbInputs));

    Sample Y1(0, nbOutputs);
    Sample Y2(0, nbOutputs);
    Collection<Sample> crossY1(nbInputs, Sample(0, nbOutputs));

    Collection<Sample> allFirstOrderIndices(nbOutputs, Sample(0, nbInputs));
    Collection<Sample> allTotalIndices(nbOutputs, Sample(0, nbInputs));

    // We loop if there remains time, some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
    while (!stopRequested_
           && (outerSampling < maximumOuterSampling)
           && ((coefficientOfVariation == -1.0) || (coefficientOfVariation > getMaximumCoefficientOfVariation()))
           &&  (static_cast<UnsignedInteger>(elapsedTime) < getMaximumElapsedTime() * CLOCKS_PER_SEC))
    {
      // progress
      if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
      {
        progressValue_ = (int) (outerSampling * 100 / maximumOuterSampling);
        notify("progressValueChanged");
      }

      // the last block can be smaller
      const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? getBlockSize() : lastBlockSize;

      // Perform two blocks of simulations
      // - first input sample
      const Sample blockInputSample1(generateInputSample(effectiveBlockSize));
      X1.add(blockInputSample1);
      // - second input sample
      const Sample blockInputSample2(generateInputSample(effectiveBlockSize));
      X2.add(blockInputSample2);

      // fill samples, inputs of the sensitivity analysis algo
      inputDesign = X1;
      inputDesign.add(X2);
      outputDesign = Sample(0, nbOutputs);

      try
      {
        // - first output sample
        const Sample blockOutputSample1(computeOutputSample(blockInputSample1));
        Y1.add(blockOutputSample1);
        // - second output sample
        const Sample blockOutputSample2(computeOutputSample(blockInputSample2));
        Y2.add(blockOutputSample2);

        outputDesign.add(Y1);
        outputDesign.add(Y2);

        // - compute designs of type Saltelli
        for (UnsignedInteger i = 0; i < nbInputs; ++i)
        {
          Sample x(blockInputSample1);
          for (UnsignedInteger j = 0; j < effectiveBlockSize; ++j)
            x(j, i) = blockInputSample2(j, i);
          crossX1[i].add(x);
          inputDesign.add(crossX1[i]);
          crossY1[i].add(computeOutputSample(x));
          outputDesign.add(crossY1[i]);
        }
      }
      catch (std::exception & ex)
      {
        throw InvalidValueException(HERE) << "An error happened when evaluating the model. " << ex.what();
      }

      // create Saltelli algo - compute indices
      if (getBlockSize() != 1 || (getBlockSize() == 1 && outerSampling)) // must have at least two values
      {
        const UnsignedInteger sampleSize(outerSampling < (maximumOuterSampling - 1) ?
                                         getBlockSize() * (outerSampling + 1) :
                                         getBlockSize() * outerSampling + lastBlockSize);

        // information message
        OSS oss;
        oss << "Number of evaluations = " << inputDesign.getSize() << "\n";
        oss << "Coefficient of variation = " << coefficientOfVariation << "\n";
        oss << "Elapsed time = " << (float) elapsedTime / CLOCKS_PER_SEC << " s\n";
        informationMessage_ = oss;
        notify("informationMessageUpdated");

        if (sampleSize == 1)
          throw InvalidValueException(HERE) << "Impossible to compute the sensitivity indices. Increase the block size and/or the maximum calls.";

        try
        {
          // build algo
          SaltelliSensitivityAlgorithm algoSaltelli(inputDesign, outputDesign, sampleSize);

          // compute indices
          for (UnsignedInteger i = 0; i < nbOutputs; ++i)
          {
            allFirstOrderIndices[i].add(algoSaltelli.getFirstOrderIndices(i));
            allTotalIndices[i].add(algoSaltelli.getTotalOrderIndices(i));
          }
        }
        catch (std::exception & ex)
        {
          throw InvalidValueException(HERE) << "An error happened when computing the sensitivity indices. " << ex.what();
        }
      }

      // stop criteria
      // - compute coefficient of variation: take into account the greatest one
      if ((getMaximumCoefficientOfVariation() != -1) && (allFirstOrderIndices[0].getSize() > 1))
      {
        Scalar coefOfVar(0.);
        for (UnsignedInteger i = 0; i < nbOutputs; ++i)
        {
          if (!allFirstOrderIndices[i].getSize())
            throw InvalidValueException(HERE) << "An error happened when computing the coefficient of variation";

          const Point empiricalMean(allFirstOrderIndices[i].computeMean());
          const Point empiricalStd(allFirstOrderIndices[i].computeStandardDeviationPerComponent());
          for (UnsignedInteger j = 0; j < nbInputs; ++j)
          {
            if (std::abs(empiricalMean[j]) < SpecFunc::Precision)
              throw InvalidValueException(HERE) << "Impossible to compute the coefficient of variation because the mean of an indice is too close to zero.\
                                                    Do not use the coefficient of variation as criteria to stop the algorithm";

            const Scalar sigma_j = empiricalStd[j] / sqrt(allFirstOrderIndices[i].getSize());
            coefOfVar = std::max(sigma_j / std::abs(empiricalMean[j]), coefOfVar);
          }
        }
        coefficientOfVariation = coefOfVar;
      }
      // - update time and number of iterations
      elapsedTime = clock() - startTime;
      ++outerSampling;
    }

    // retrieve the last values of indices
    Sample firstOrderIndices(0, nbInputs);
    firstOrderIndices.setDescription(getPhysicalModel().getStochasticInputNames());
    Sample totalIndices(0, nbInputs);
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      if (!(allFirstOrderIndices[i].getSize() * allTotalIndices[i].getSize()))
        throw InvalidValueException(HERE) << "No result. Try to increase the block size and/or the maximum calls.";
      firstOrderIndices.add(allFirstOrderIndices[i][allFirstOrderIndices[i].getSize() - 1]);
      totalIndices.add(allTotalIndices[i][allTotalIndices[i].getSize() - 1]);
    }

    // compute indices interval
    const UnsignedInteger sampleSize = inputDesign.getSize() / (2 + nbInputs);

    Collection<Interval> foIntervals;
    Collection<Interval> toIntervals;
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      SaltelliSensitivityAlgorithm algoSaltelli(inputDesign, outputDesign.getMarginal(i), sampleSize);
      algoSaltelli.setBootstrapSize(bootstrapSize_);
      algoSaltelli.setBootstrapConfidenceLevel(bootstrapConfidenceLevel_);
      foIntervals.add(algoSaltelli.getFirstOrderIndicesInterval());
      toIntervals.add(algoSaltelli.getTotalOrderIndicesInterval());
    }

    // fill result_
    result_ = SobolResult(firstOrderIndices, totalIndices, getInterestVariables());
    result_.firstOrderIndicesInterval_ = foIntervals;
    result_.totalIndicesInterval_ = toIntervals;
    result_.callsNumber_ = X1.getSize() * (2 + nbInputs);
    result_.elapsedTime_ = (float) elapsedTime / CLOCKS_PER_SEC;
    result_.coefficientOfVariation_ = coefficientOfVariation;

    // add warning if the model does not have an independent copula
    if (!getPhysicalModel().getComposedDistribution().hasIndependentCopula())
    {
      LOGWARN("The model does not have an independent copula, the result of the sensitivity analysis could be false.");
      warningMessage_ = "The model does not have an independent copula, the result of the sensitivity analysis could be false.";
    }

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
  isRunning_ = false;
}


SobolResult SobolAnalysis::getResult() const
{
  return result_;
}


Parameters SobolAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Sobol");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Maximum coefficient of variation", getMaximumCoefficientOfVariation());
  String time = "- (s)";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    time = (OSS() << getMaximumElapsedTime()).str() + "(s)";
  param.add("Maximum elapsed time", time);
  String maxCalls = "-";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    maxCalls = (OSS() << getMaximumCalls()).str();
  param.add("Maximum calls", maxCalls);
  param.add("Block size", getBlockSize());
  param.add("Seed", getSeed());
  param.add("Bootstrap size", getBootstrapSize());
  param.add("Bootstrap confidence level", getBootstrapConfidenceLevel());

  return param;
}


String SobolAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.SobolAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";
  oss << getName() << ".setBootstrapSize(" << getBootstrapSize() << ")\n";
  oss << getName() << ".setBootstrapConfidenceLevel(" << getBootstrapConfidenceLevel() << ")\n";

  return oss;
}


bool SobolAnalysis::hasValidResult() const
{
  return result_.getFirstOrderIndices().getSize() != 0;
}


/* String converter */
String SobolAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << WithStopCriteriaAnalysis::__repr__()
      << " seed=" << getSeed()
      << " blockSize=" << getBlockSize()
      << " bootstrapSize=" << getBootstrapSize()
      << " bootstrapConfidenceLevel=" << getBootstrapConfidenceLevel();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SobolAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
  adv.saveAttribute("bootstrapConfidenceLevel_", bootstrapConfidenceLevel_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SobolAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
  adv.loadAttribute("bootstrapConfidenceLevel_", bootstrapConfidenceLevel_);
  adv.loadAttribute("result_", result_);
}
}
