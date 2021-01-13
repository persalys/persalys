//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the Importance sampling method
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/ImportanceSamplingAnalysis.hxx"

#include <openturns/ProbabilitySimulationAlgorithm.hxx>
#include <openturns/Normal.hxx>
#include <openturns/StandardEvent.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ImportanceSamplingExperiment.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ImportanceSamplingAnalysis)

static Factory<ImportanceSamplingAnalysis> Factory_ImportanceSamplingAnalysis;

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
  , standardSpaceDesignPoint_(Point(limitState.getPhysicalModel().getStochasticInputNames().getSize()))
{
}


/* Virtual constructor */
ImportanceSamplingAnalysis* ImportanceSamplingAnalysis::clone() const
{
  return new ImportanceSamplingAnalysis(*this);
}


SimulationInterface ImportanceSamplingAnalysis::getSimulationAlgorithm(const RandomVector & event)
{
  const UnsignedInteger inDimension = standardSpaceDesignPoint_.getSize();
  const Normal conditionalDistribution(standardSpaceDesignPoint_, Point(inDimension, 1.), CorrelationMatrix(inDimension));
  const ImportanceSamplingExperiment experiment(conditionalDistribution);

  return ProbabilitySimulationAlgorithm(StandardEvent(event), experiment);
}


Point ImportanceSamplingAnalysis::getStandardSpaceDesignPoint() const
{
  return standardSpaceDesignPoint_;
}


void ImportanceSamplingAnalysis::setStandardSpaceDesignPoint(const Point& point)
{
  standardSpaceDesignPoint_ = point;
}


Parameters ImportanceSamplingAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Importance sampling");
  param.add("Output of interest", getLimitState().getOutputName());
  param.add("Design point (standard space)", getStandardSpaceDesignPoint());
  param.add(WithStopCriteriaAnalysis::getParameters());
  param.add(SimulationReliabilityAnalysis::getParameters());

  return param;
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
