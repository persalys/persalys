//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "otgui/ProbabilisticDesignOfExperiment.hxx"

#include <openturns/MonteCarloExperiment.hxx>
#include <openturns/LHSExperiment.hxx>
#include <openturns/SobolSequence.hxx>
#include <openturns/LowDiscrepancyExperiment.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ProbabilisticDesignOfExperiment);

static Factory<ProbabilisticDesignOfExperiment> Factory_ProbabilisticDesignOfExperiment;

Description ProbabilisticDesignOfExperiment::DesignNames_;

Description ProbabilisticDesignOfExperiment::GetDesignNames()
{
  if (DesignNames_.isEmpty())
  {
    DesignNames_ = Description();
    DesignNames_.add("LHS");
    DesignNames_.add("MONTE_CARLO");
    DesignNames_.add("QUASI_MONTE_CARLO");
  }

  return DesignNames_;
}


ProbabilisticDesignOfExperiment::ProbabilisticDesignOfExperiment()
  : DesignOfExperimentImplementation()
  , designName_("LHS")
  , size_(ResourceMap::GetAsUnsignedInteger("WeightedExperiment-DefaultSize"))
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
}


ProbabilisticDesignOfExperiment::ProbabilisticDesignOfExperiment(const String& name,
                                                                 const PhysicalModel& physicalModel,
                                                                 const UnsignedInteger size,
                                                                 const String& designName)
  : DesignOfExperimentImplementation(name, physicalModel)
  , designName_("")
  , size_(0)
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
  setDesignName(designName);
  setSize(size);
}


ProbabilisticDesignOfExperiment* ProbabilisticDesignOfExperiment::clone() const
{
  return new ProbabilisticDesignOfExperiment(*this);
}


Sample ProbabilisticDesignOfExperiment::getInputSample() const
{
  if (!inputSample_.getSize())
    inputSample_ = GenerateInputSample(getDesignName(), getSize(), getPhysicalModel(), getSeed());

  return inputSample_;
}


String ProbabilisticDesignOfExperiment::getDesignName() const
{
  return designName_;
}


void ProbabilisticDesignOfExperiment::setDesignName(const String& name)
{
  if (!GetDesignNames().contains(name))
    throw InvalidArgumentException(HERE) << "Error: the given design of experiment name=" << name << " is unknown.";

  designName_ = name;

  // clear samples
  inputSample_.clear();
  initialize();
}


UnsignedInteger ProbabilisticDesignOfExperiment::getSize() const
{
  return size_;
}


void ProbabilisticDesignOfExperiment::setSize(const UnsignedInteger size)
{
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: the size must be > 0.";

  size_ = size;

  // clear samples
  inputSample_.clear();
  initialize();
}


UnsignedInteger ProbabilisticDesignOfExperiment::getSeed() const
{
  return seed_;
}


void ProbabilisticDesignOfExperiment::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;

  // clear samples
  inputSample_.clear();
  initialize();
}


Sample ProbabilisticDesignOfExperiment::GenerateInputSample(const String& name,
                                                            const UnsignedInteger size,
                                                            const PhysicalModel& model,
                                                            const UnsignedInteger seed)
{
  Sample sample;

  RandomGenerator::SetSeed(seed);

  if (name == "LHS")
    sample = LHSExperiment(model.getComposedDistribution(), size).generate();
  else if (name == "MONTE_CARLO")
    sample = MonteCarloExperiment(model.getComposedDistribution(), size).generate();
  else if (name == "QUASI_MONTE_CARLO")
    sample = LowDiscrepancyExperiment(SobolSequence(model.getStochasticInputNames().getSize()), model.getComposedDistribution(), size).generate();
  else
    throw InvalidArgumentException(HERE) << "Error: GenerateInputSample design name unknown";

  // if there is at least a deterministic variable
  if (model.getStochasticInputNames().getSize() < model.getInputNames().getSize())
  {
    Point inValues(model.getInputs().getSize());
    Indices variableInputsIndices;
    for (UnsignedInteger i=0; i<inValues.getSize(); ++i)
    {
      inValues[i] = model.getInputs()[i].getValue();
      if (model.getInputs()[i].isStochastic())
        variableInputsIndices.add(i);
    }

    Sample inputSample(size, inValues);
    for (UnsignedInteger i=0; i<size; ++i)
    {
      for (UnsignedInteger j=0; j<variableInputsIndices.getSize(); ++j)
      {
        inputSample[i][variableInputsIndices[j]] = sample[i][j];
      }
    }
    inputSample.setDescription(model.getInputNames());
    return inputSample;
  }
  else
  {
    sample.setDescription(model.getInputNames());
    return sample;
  }
}


String ProbabilisticDesignOfExperiment::getPythonScript() const
{
  OSS oss;
  oss << getName() + " = otguibase.ProbabilisticDesignOfExperiment('" + getName() + "', "+getPhysicalModel().getName()+", ";
  oss << getSize() << ", '" << getDesignName() << "')\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

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
      << " seed= " << getSeed();

  return oss;
}


/* Method save() stores the object through the StorageManager */
void ProbabilisticDesignOfExperiment::save(Advocate& adv) const
{
  DesignOfExperimentImplementation::save(adv);
  adv.saveAttribute("designName_", designName_);
  adv.saveAttribute("size_", size_);
  adv.saveAttribute("seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void ProbabilisticDesignOfExperiment::load(Advocate& adv)
{
  DesignOfExperimentImplementation::load(adv);
  adv.loadAttribute("designName_", designName_);
  adv.loadAttribute("size_", size_);
  adv.loadAttribute("seed_", seed_);
}
}