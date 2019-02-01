//                                               -*- C++ -*-
/**
 *  @brief SRCAnalysis computes the Standard Regression Coefficients
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
#include "otgui/SRCAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/CorrelationAnalysis.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(SRCAnalysis)

static Factory<SRCAnalysis> Factory_SRCAnalysis;

/* Default constructor */
SRCAnalysis::SRCAnalysis()
  : SimulationAnalysis()
  , simulationsNumber_(10000)
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
SRCAnalysis::SRCAnalysis(const String & name, const PhysicalModel & physicalModel, const UnsignedInteger nbSimu)
  : SimulationAnalysis(name, physicalModel)
  , simulationsNumber_(nbSimu)
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Virtual constructor */
SRCAnalysis* SRCAnalysis::clone() const
{
  return new SRCAnalysis(*this);
}


UnsignedInteger SRCAnalysis::getSimulationsNumber() const
{
  return simulationsNumber_;
}


void SRCAnalysis::setSimulationsNumber(const UnsignedInteger number)
{
  simulationsNumber_ = number;
}


void SRCAnalysis::initialize()
{
  SimulationAnalysis::initialize();
  result_ = SRCResult();
}


void SRCAnalysis::launch()
{
  // check
  if (getBlockSize() > getSimulationsNumber())
    throw InvalidValueException(HERE) << "The block size (" << getBlockSize()
                                      << ") can not be greater than the input sample size (" << getSimulationsNumber() << ")";

  // initialization
  RandomGenerator::SetSeed(getSeed());

  const Sample inputSample(generateInputSample(getSimulationsNumber()));
  const UnsignedInteger inputSampleSize = inputSample.getSize();

  // number of iterations
  const UnsignedInteger nbIter = static_cast<UnsignedInteger>(ceil(1.0 * inputSampleSize / getBlockSize()));
  // last block size
  const UnsignedInteger modulo = inputSampleSize % getBlockSize();
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  // evaluate model
  Sample outputSample(0, getInterestVariables().getSize());
  for (UnsignedInteger i = 0; i < nbIter; ++i)
  {
    if (stopRequested_ && i > 1)
      break;

    progressValue_ = (int) (i * 100 / nbIter);
    notify("progressValueChanged");

    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = i < (nbIter - 1) ? getBlockSize() : lastBlockSize;

    // get input sample of size effectiveBlockSize
    const UnsignedInteger blockFirstIndex =  i * getBlockSize();
    const Sample blockInputSample(inputSample, blockFirstIndex, blockFirstIndex + effectiveBlockSize);

    // Perform a block of simulations
    outputSample.add(computeOutputSample(blockInputSample));
  }

  // compute SRC indices
  Sample indices(0, inputSample.getDimension());

  const Sample effectiveInputSample(inputSample, 0, outputSample.getSize());
  for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
  {
    indices.add(CorrelationAnalysis::SRC(effectiveInputSample, outputSample.getMarginal(i)));
  }

  indices.setDescription(inputSample.getDescription());
  // set results
  result_.indices_ = indices;
  result_.outputNames_ = getInterestVariables();

  // add warning if the model does not have an independent copula
  if (!getPhysicalModel().getCopula().hasIndependentCopula())
  {
    LOGWARN("The model does not have an independent copula, the result of the sensitivity analysis could be false.");
    warningMessage_ = "The model does not have an independent copula, the result of the sensitivity analysis could be false.";
  }
}


SRCResult SRCAnalysis::getResult() const
{
  return result_;
}


Parameters SRCAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Standardized Regression Coefficients");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getSimulationsNumber());
  param.add("Block size", getBlockSize());
  param.add("Seed", getSeed());

  return param;
}


String SRCAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.SRCAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getSimulationsNumber() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SRCAnalysis::hasValidResult() const
{
  return result_.getIndices().getSize() != 0;
}


/* String converter */
String SRCAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << " simulationsNumber=" << getSimulationsNumber()
      << " seed=" << getSeed()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SRCAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("simulationsNumber_", simulationsNumber_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SRCAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("simulationsNumber_", simulationsNumber_);
  adv.loadAttribute("result_", result_);
}
}
