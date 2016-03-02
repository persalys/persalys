//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
 *
 *  Copyright 2015-2016 EDF
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

#include "RandomGenerator.hxx"
#include "SensitivityAnalysis.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolAnalysis);

static Factory<SobolAnalysis> RegisteredFactory("SobolAnalysis");

/* Default constructor */
SobolAnalysis::SobolAnalysis()
  : SimulationAnalysis()
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
{
}


/* Constructor with parameters */
SobolAnalysis::SobolAnalysis(const String & name, const PhysicalModel & physicalModel, const UnsignedInteger nbSimu)
  : SimulationAnalysis(name, physicalModel, nbSimu)
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
  NumericalSample inputSample1(getInputSample());
  NumericalSample inputSample2(getInputSample());

  SensitivityAnalysis sensitivityAnalysis = SensitivityAnalysis(inputSample1, inputSample2, getPhysicalModel().getRestrictedFunction(getOutputNames()));
  sensitivityAnalysis.setBlockSize(blockSize_);

  // set results
  Collection<SymmetricMatrix> secondOrderIndices;
  secondOrderIndices.add(sensitivityAnalysis.getSecondOrderIndices(0));
  NumericalSample firstOrderIndices(1, sensitivityAnalysis.getFirstOrderIndices(0));
  NumericalSample totalOrderIndices(1, sensitivityAnalysis.getTotalOrderIndices(0));

  for (UnsignedInteger i=1; i<getOutputs().getSize(); ++i)
  {
    secondOrderIndices.add(sensitivityAnalysis.getSecondOrderIndices(i));
    firstOrderIndices.add(sensitivityAnalysis.getFirstOrderIndices(i));
    totalOrderIndices.add(sensitivityAnalysis.getTotalOrderIndices(i));
  }

  firstOrderIndices.setDescription(getPhysicalModel().getStochasticInputNames());
  result_ = SobolResult(firstOrderIndices, secondOrderIndices, totalOrderIndices, getOutputNames());

  notify("analysisFinished");
}


UnsignedInteger SobolAnalysis::getBlockSize() const
{
  return blockSize_;
}


void SobolAnalysis::setBlockSize(const UnsignedInteger & size)
{
  blockSize_ = size;
}


SobolResult SobolAnalysis::getResult() const
{
  return result_;
}


String SobolAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.SobolAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";
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
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SobolAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("result_", result_);
}
}