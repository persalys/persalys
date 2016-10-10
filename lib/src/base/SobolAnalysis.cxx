//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
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
#include "otgui/SobolAnalysis.hxx"

#include "openturns/RandomGenerator.hxx"
#include "openturns/SaltelliSensitivityAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolAnalysis);

static Factory<SobolAnalysis> RegisteredFactory;

/* Default constructor */
SobolAnalysis::SobolAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
{
}


/* Constructor with parameters */
SobolAnalysis::SobolAnalysis(const String & name, const PhysicalModel & physicalModel)
  : SimulationAnalysis(name, physicalModel)
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


/* Virtual constructor */
SobolAnalysis* SobolAnalysis::clone() const
{
  return new SobolAnalysis(*this);
}


void SobolAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());

  const UnsignedInteger nbInputs(getPhysicalModel().getStochasticInputNames().getSize());
  const UnsignedInteger nbOutputs(getPhysicalModel().getOutputNames().getSize()); // TODO only required outputs

  const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / (getBlockSize()*(2+nbInputs)))) : (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getMaximumCalls() % (getBlockSize()*(2+nbInputs)) : 0;
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  NumericalScalar coefficientOfVariation = -1.0;
  clock_t elapsedTime = 0;
  const clock_t startTime = clock();
  UnsignedInteger outerSampling = 0;

  NumericalSample X1(0, nbInputs);
  NumericalSample X2(0, nbInputs);
  Collection<NumericalSample> crossX1(nbInputs, NumericalSample(0, nbInputs));

  NumericalSample Y1(0, nbOutputs);
  NumericalSample Y2(0, nbOutputs);
  Collection<NumericalSample> crossY1(nbInputs, NumericalSample(0, nbOutputs));

  Collection<NumericalSample> allFirstOrderIndices(nbOutputs, NumericalSample(0, nbInputs));
  Collection<NumericalSample> allTotalIndices(nbOutputs, NumericalSample(0, nbInputs));
//   Interval firstOrderIndicesInterval;
//   Interval totalIndicesInterval;
  SaltelliSensitivityAlgorithm algoSaltelli;

  // We loop if there remains time, some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while ((outerSampling < maximumOuterSampling)
     && ((coefficientOfVariation == -1.0) || (coefficientOfVariation > getMaximumCoefficientOfVariation()))
     &&  (elapsedTime < getMaximumElapsedTime()*CLOCKS_PER_SEC))
  {
    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? getBlockSize() : lastBlockSize;

    // Perform two blocks of simulations
    // - first sample
    const NumericalSample blockInputSample1(generateInputSample(effectiveBlockSize));
    const NumericalSample blockOutputSample1(computeOutputSample(blockInputSample1));
    X1.add(blockInputSample1);
    Y1.add(blockOutputSample1);
    // - second sample
    const NumericalSample blockInputSample2(generateInputSample(effectiveBlockSize));
    const NumericalSample blockOutputSample2(computeOutputSample(blockInputSample2));
    X2.add(blockInputSample2);
    Y2.add(blockOutputSample2);

    // fill samples, inputs of the sensitivity analysis algo
    NumericalSample inputDesign(X1);
    inputDesign.add(X2);
    NumericalSample outputDesign(Y1);
    outputDesign.add(Y2);

    // - compute designs of type Saltelli
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      NumericalSample x(blockInputSample1);
      for (UnsignedInteger j = 0; j < effectiveBlockSize; ++j)
        x[j][i] = blockInputSample2[j][i];
      crossX1[i].add(x);
      inputDesign.add(crossX1[i]);
      crossY1[i].add(computeOutputSample(x));
      outputDesign.add(crossY1[i]);
    }

    // create Saltelli algo - compute indices
    if (getBlockSize() != 1 || (getBlockSize() == 1 && outerSampling)) // must have at least two values
    {
      const UnsignedInteger sampleSize(outerSampling < (maximumOuterSampling - 1) ?
                                       effectiveBlockSize*(outerSampling+1) :
                                       effectiveBlockSize*outerSampling+lastBlockSize);

      algoSaltelli = SaltelliSensitivityAlgorithm(inputDesign, outputDesign, sampleSize);

      for (UnsignedInteger i=0; i<nbOutputs; ++i)
      {
        allFirstOrderIndices[i].add(algoSaltelli.getFirstOrderIndices(i));
        allTotalIndices[i].add(algoSaltelli.getTotalOrderIndices(i));
      }
    }

    // stop criteria
    // - compute coefficient of variation: take into account the greatest one
    if ((getMaximumCoefficientOfVariation() != -1) && (allFirstOrderIndices[0].getSize() > 1))
    {
      NumericalScalar coefOfVar(0.);
      for (UnsignedInteger i=0; i<nbOutputs; ++i)
      {
        const NumericalPoint empiricalMean = allFirstOrderIndices[i].computeMean();
        const NumericalPoint empiricalStd = allFirstOrderIndices[i].computeStandardDeviationPerComponent();
        for (UnsignedInteger j=0; j<nbInputs; ++j)
        {
          const NumericalScalar sigma_j = empiricalStd[j] / sqrt(allFirstOrderIndices[i].getSize());
          coefOfVar = std::max(sigma_j / empiricalMean[j], coefOfVar);
        }
      }
      coefficientOfVariation = coefOfVar;
    }
    // - update time and number of iterations
    elapsedTime = clock() - startTime;
    ++outerSampling;
  }

  // retrieve the last values of indices
  NumericalSample firstOrderIndices(0, nbInputs);
  firstOrderIndices.setDescription(getPhysicalModel().getStochasticInputNames());
  NumericalSample totalIndices(0, nbInputs);
  for (UnsignedInteger i=0; i<nbOutputs; ++i)
  {
    firstOrderIndices.add(allFirstOrderIndices[i][allFirstOrderIndices[i].getSize()-1]);
    totalIndices.add(allTotalIndices[i][allTotalIndices[i].getSize()-1]);
  }

  // compute indices interval
//   algoSaltelli.setBootstrapSize(1000);
//   firstOrderIndicesInterval = algoSaltelli.getFirstOrderIndicesInterval();
//   totalIndicesInterval = algoSaltelli.getTotalOrderIndicesInterval();

  // fill result_
  result_ = SobolResult(firstOrderIndices, totalIndices, getOutputNames());
  result_.setCallsNumber(X1.getSize()*(2+nbInputs));
  result_.setElapsedTime((float)elapsedTime/CLOCKS_PER_SEC);
  result_.setCoefficientOfVariation(coefficientOfVariation);

  // add warning if the model has not an independent copula
  if (!getPhysicalModel().getComposedDistribution().hasIndependentCopula())
  {
    LOGWARN("The model has not an independent copula, the result of the sensitivity analysis could be false.");
  }

  notify("analysisFinished");
}


SobolResult SobolAnalysis::getResult() const
{
  return result_;
}


String SobolAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.SobolAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SobolAnalysis::analysisLaunched() const
{
  return result_.getFirstOrderIndices().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void SobolAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SobolAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("result_", result_);
}
}
