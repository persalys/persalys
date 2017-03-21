//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the Importance sampling method
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
#include "otgui/ImportanceSamplingAnalysis.hxx"

#include <openturns/ImportanceSampling.hxx>
#include <openturns/Normal.hxx>
#include <openturns/StandardEvent.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ImportanceSamplingAnalysis);

static Factory<ImportanceSamplingAnalysis> RegisteredFactory;

/* Default constructor */
ImportanceSamplingAnalysis::ImportanceSamplingAnalysis()
  : SimulationReliabilityAnalysis()
  , standardSpaceDesignPoint_()
{
}


/* Constructor with parameters */
ImportanceSamplingAnalysis::ImportanceSamplingAnalysis(const String& name,
                                                       const LimitState& limitState)
  : SimulationReliabilityAnalysis(name, limitState)
  , standardSpaceDesignPoint_(NumericalPoint(limitState.getPhysicalModel().getStochasticInputNames().getSize()))
{
}


/* Virtual constructor */
ImportanceSamplingAnalysis* ImportanceSamplingAnalysis::clone() const
{
  return new ImportanceSamplingAnalysis(*this);
}


Simulation* ImportanceSamplingAnalysis::getSimulationAlgorithm(const Event& event)
{
  const UnsignedInteger inDimension = standardSpaceDesignPoint_.getSize();
  const Normal conditionalDistribution(standardSpaceDesignPoint_, NumericalPoint(inDimension, 1.), CorrelationMatrix(inDimension));

  return new ImportanceSampling(StandardEvent(event), conditionalDistribution);
}


NumericalPoint ImportanceSamplingAnalysis::getStandardSpaceDesignPoint() const
{
  return standardSpaceDesignPoint_;
}


void ImportanceSamplingAnalysis::setStandardSpaceDesignPoint(const NumericalPoint& point)
{
  standardSpaceDesignPoint_ = point;
}


String ImportanceSamplingAnalysis::getPythonScript() const
{
  OSS oss;
  oss << SimulationReliabilityAnalysis::getPythonScript();
  oss << getName() << ".setStandardSpaceDesignPoint(" << getStandardSpaceDesignPoint().__str__() << ")\n";

  return oss;
}


/* String converter */
String ImportanceSamplingAnalysis::__repr__() const
{
  OSS oss;
  oss << SimulationReliabilityAnalysis::__repr__()
      << " standardSpaceDesignPoint=" << getStandardSpaceDesignPoint();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportanceSamplingAnalysis::save(Advocate & adv) const
{
  SimulationReliabilityAnalysis::save(adv);
  adv.saveAttribute("standardSpaceDesignPoint_", standardSpaceDesignPoint_);
}


/* Method load() reloads the object from the StorageManager */
void ImportanceSamplingAnalysis::load(Advocate & adv)
{
  SimulationReliabilityAnalysis::load(adv);
  adv.loadAttribute("standardSpaceDesignPoint_", standardSpaceDesignPoint_);
}
}