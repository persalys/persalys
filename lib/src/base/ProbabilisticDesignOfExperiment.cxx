//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/ProbabilisticDesignOfExperiment.hxx"

#include <openturns/MonteCarloExperiment.hxx>
#include <openturns/LHSExperiment.hxx>
#include <openturns/SobolSequence.hxx>
#include <openturns/LowDiscrepancyExperiment.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SimulatedAnnealingLHS.hxx>
#include <openturns/MonteCarloLHS.hxx>
#include <openturns/SpaceFillingPhiP.hxx>
#include <openturns/SpaceFillingMinDist.hxx>
#include <openturns/SpaceFillingC2.hxx>


using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ProbabilisticDesignOfExperiment)

static Factory<ProbabilisticDesignOfExperiment> Factory_ProbabilisticDesignOfExperiment;

Description ProbabilisticDesignOfExperiment::DesignNames_;
Description ProbabilisticDesignOfExperiment::SpaceFillings_;

Description ProbabilisticDesignOfExperiment::GetDesignNames()
{
  if (DesignNames_.isEmpty())
  {
    DesignNames_ = Description();
    DesignNames_.add("LHS");
    DesignNames_.add("SALHS");
    DesignNames_.add("MCLHS");
    DesignNames_.add("MONTE_CARLO");
    DesignNames_.add("QUASI_MONTE_CARLO");
  }

  return DesignNames_;
}

Description ProbabilisticDesignOfExperiment::GetSpaceFillings()
{
  if (SpaceFillings_.isEmpty())
  {
    SpaceFillings_ = Description();
    SpaceFillings_.add("PhiP");
    SpaceFillings_.add("minDist");
    SpaceFillings_.add("C2");
  }

  return SpaceFillings_;
}

ProbabilisticDesignOfExperiment::ProbabilisticDesignOfExperiment()
  : DesignOfExperimentEvaluation()
  , designName_("LHS")
  , spaceFilling_("PhiP")
  , size_(ResourceMap::GetAsUnsignedInteger("WeightedExperiment-DefaultSize"))
{
  isDeterministicAnalysis_ = false;
}


ProbabilisticDesignOfExperiment::ProbabilisticDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const UnsignedInteger size,
    const String& designName, const String& spaceFilling, const UnsignedInteger mcLhsSize)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , designName_("")
  , spaceFilling_("")
  , size_(0)
  , mcLhsSize_(mcLhsSize)
{
  isDeterministicAnalysis_ = false;
  setDesignName(designName);
  setSpaceFilling(spaceFilling);
  setSize(size);
}


ProbabilisticDesignOfExperiment* ProbabilisticDesignOfExperiment::clone() const
{
  return new ProbabilisticDesignOfExperiment(*this);
}


String ProbabilisticDesignOfExperiment::getDesignName() const
{
  return designName_;
}


void ProbabilisticDesignOfExperiment::setDesignName(const String& name)
{
  if (!GetDesignNames().contains(name))
    throw InvalidArgumentException(HERE) << "Error: the given design of experiments name=" << name << " is unknown.";

  designName_ = name;

  // clear samples
  originalInputSample_.clear();
  initialize();
}

String ProbabilisticDesignOfExperiment::getSpaceFilling() const
{
  return spaceFilling_;
}

void ProbabilisticDesignOfExperiment::setSpaceFilling(const String& name)
{
  if (!GetSpaceFillings().contains(name))
    throw InvalidArgumentException(HERE) << "Error: the given design of experiments space filling algorithm=" << name << " is unknown.";

  spaceFilling_ = name;
}

UnsignedInteger ProbabilisticDesignOfExperiment::getSize() const
{
  return size_;
}

UnsignedInteger ProbabilisticDesignOfExperiment::getMCLHSSize() const
{
  return mcLhsSize_;
}

void ProbabilisticDesignOfExperiment::setSize(const UnsignedInteger size)
{
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: the size must be > 0.";

  size_ = size;

  // clear samples
  originalInputSample_.clear();
  initialize();
}


void ProbabilisticDesignOfExperiment::setSeed(const UnsignedInteger seed)
{
  SimulationAnalysis::setSeed(seed);

  // clear samples
  originalInputSample_.clear();
  initialize();
}


Sample ProbabilisticDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  Sample sample;

  RandomGenerator::SetSeed(getSeed());
  SpaceFilling spaceFilling;

  if (designName_ == "SALHS" || designName_ == "MCLHS") {
    if (spaceFilling_ == "PhiP")
      spaceFilling = SpaceFillingPhiP();
    else if (spaceFilling_ == "minDist")
      spaceFilling = SpaceFillingMinDist();
    else if (spaceFilling_ == "C2")
      spaceFilling = SpaceFillingC2();
    else
      throw InvalidArgumentException(HERE) << "Error: generateInputSample space filling algorithm name unknown";
  }

  if (designName_ == "LHS")
    sample = LHSExperiment(getPhysicalModel().getDistribution(), size_).generate();
  else if (designName_ == "SALHS")
    sample = SimulatedAnnealingLHS(LHSExperiment(getPhysicalModel().getDistribution(), size_), spaceFilling, GeometricProfile()).generate();
  else if (designName_ == "MCLHS")
    sample = MonteCarloLHS(LHSExperiment(getPhysicalModel().getDistribution(), size_, true), mcLhsSize_, spaceFilling).generate();
  else if (designName_ == "MONTE_CARLO")
    sample = MonteCarloExperiment(getPhysicalModel().getDistribution(), size_).generate();
  else if (designName_ == "QUASI_MONTE_CARLO")
    sample = LowDiscrepancyExperiment(SobolSequence(getPhysicalModel().getStochasticInputNames().getSize()), getPhysicalModel().getDistribution(), size_).generate();
  else
    throw InvalidArgumentException(HERE) << "Error: generateInputSample design name unknown";

  // if there is at least a deterministic variable
  if (getPhysicalModel().getStochasticInputNames().getSize() < getPhysicalModel().getInputDimension())
  {
    Point inValues(getPhysicalModel().getInputDimension());
    Indices variableInputsIndices;
    for (UnsignedInteger i = 0; i < inValues.getSize(); ++i)
    {
      inValues[i] = getPhysicalModel().getInputs()[i].getValue();
      if (getPhysicalModel().getInputs()[i].isStochastic())
        variableInputsIndices.add(i);
    }

    Sample inputSample(size_, inValues);
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      for (UnsignedInteger j = 0; j < variableInputsIndices.getSize(); ++j)
      {
        inputSample(i, variableInputsIndices[j]) = sample(i, j);
      }
    }
    inputSample.setDescription(getPhysicalModel().getInputNames());
    return inputSample;
  }
  else
  {
    sample.setDescription(getPhysicalModel().getInputNames());
    return sample;
  }
}


Parameters ProbabilisticDesignOfExperiment::getParameters() const
{
  Parameters param;

  String designName = "LHS";
  if (getDesignName() == "SALHS")
    designName = "Simulated annealing LHS";
  else if (getDesignName() == "MCLHS")
    designName = "Monte Carlo LHS";
  else if (getDesignName() == "MONTE_CARLO")
    designName = "Monte Carlo";
  else if (getDesignName() == "QUASI_MONTE_CARLO")
    designName = "Quasi-Monte Carlo";
  param.add("Design name", designName);

  if (getDesignName() == "SALHS" || getDesignName() == "MCLHS") {
    String spaceFilling = "PhiP";
    if(getSpaceFilling() == "minDist")
      spaceFilling = "minDist";
    else if(getSpaceFilling() == "C2")
      spaceFilling = "C2";
    param.add("Space filling", spaceFilling);
  }

  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());
  param.add(SimulationAnalysis::getParameters());

  return param;
}


String ProbabilisticDesignOfExperiment::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.ProbabilisticDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
  oss << getSize() << ", '" << getDesignName() << "')\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";

  return oss;
}


/* String converter */
String ProbabilisticDesignOfExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " design name=" << getDesignName()
      << " size= " << getSize()
      << " seed= " << getSeed()
      << " blockSize=" << getBlockSize();

  return oss;
}


/* Method save() stores the object through the StorageManager */
void ProbabilisticDesignOfExperiment::save(Advocate& adv) const
{
  DesignOfExperimentEvaluation::save(adv);
  adv.saveAttribute("designName_", designName_);
  adv.saveAttribute("spaceFilling_", spaceFilling_);
  adv.saveAttribute("size_", size_);
  adv.saveAttribute("mcLhsSize_", mcLhsSize_);
}


/* Method load() reloads the object from the StorageManager */
void ProbabilisticDesignOfExperiment::load(Advocate& adv)
{
  DesignOfExperimentEvaluation::load(adv);
  adv.loadAttribute("designName_", designName_);
  adv.loadAttribute("size_", size_);
  if(adv.hasAttribute("spaceFilling_"))
     adv.loadAttribute("spaceFilling_", spaceFilling_);
  if(adv.hasAttribute("mcLhsSize_"))
     adv.loadAttribute("mcLhsSize_", mcLhsSize_);
}
}
