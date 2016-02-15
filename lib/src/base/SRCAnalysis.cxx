//                                               -*- C++ -*-
/**
 *  @brief SRCAnalysis computes the Standard Regression Coefficients
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
#include "otgui/SRCAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SRCAnalysis);

static Factory<SRCAnalysis> RegisteredFactory("SRCAnalysis");

/* Default constructor */
SRCAnalysis::SRCAnalysis()
  : SimulationAnalysis()
{
}


/* Constructor with parameters */
SRCAnalysis::SRCAnalysis(const String & name, const PhysicalModel & physicalModel, const UnsignedInteger nbSimu)
  : SimulationAnalysis(name, physicalModel, nbSimu)
{
}


/* Virtual constructor */
SRCAnalysis* SRCAnalysis::clone() const
{
  return new SRCAnalysis(*this);
}


void SRCAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());
  NumericalSample inputSample(getInputSample());

  // set results
  NumericalSample indices(0, inputSample.getDimension());

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
  {
    Description outputName(1);
    outputName[0] = getOutputs()[i].getName();
    indices.add(CorrelationAnalysis::SRC(inputSample, getOutputSample(inputSample, outputName)));
  }

  indices.setDescription(inputSample.getDescription());
  result_ = SRCResult(indices, getOutputNames());

  notify("analysisFinished");
}


SRCResult SRCAnalysis::getResult() const
{
  return result_;
}


String SRCAnalysis::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.SRCAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SRCAnalysis::analysisLaunched() const
{
  return result_.getIndices().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void SRCAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SRCAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("result_", result_);
}
}