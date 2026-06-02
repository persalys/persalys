//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the Importance sampling method
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/BaseTools.hxx"

#include <openturns/ProbabilitySimulationAlgorithm.hxx>
#include <openturns/Normal.hxx>
#include <openturns/StandardEvent.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ImportanceSamplingExperiment.hxx>
#include <openturns/Mixture.hxx>
#include <openturns/ComposedFunction.hxx>
#include <openturns/CompositeRandomVector.hxx>
#include <openturns/ThresholdEvent.hxx>
#include <openturns/UnionEvent.hxx>
#include <openturns/IntersectionEvent.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ImportanceSamplingAnalysis)

const static Factory<ImportanceSamplingAnalysis> Factory_ImportanceSamplingAnalysis;

/* Constructor with parameters */
ImportanceSamplingAnalysis::ImportanceSamplingAnalysis(const String& name,
    const LimitState& limitState)
  : SimulationReliabilityAnalysis(name, limitState)
{
}


/* Virtual constructor */
ImportanceSamplingAnalysis* ImportanceSamplingAnalysis::clone() const
{
  return new ImportanceSamplingAnalysis(*this);
}


SimulationInterface ImportanceSamplingAnalysis::getSimulationAlgorithm(const RandomVector & event)
{

  if (!standardSpaceDesignPoints_.getSize())
    throw InvalidArgumentException(HERE) << "No design point provided for importance sampling.";

  const UnsignedInteger inDimension = standardSpaceDesignPoints_.getDimension();

  Distribution conditionalDistribution;
  if (standardSpaceDesignPoints_.getSize() > 1)
  {
    // Union event: one design point per sub-event, use a Mixture as importance distribution
    Collection<Distribution> distributions(standardSpaceDesignPoints_.getSize());
    for (UnsignedInteger i = 0; i < standardSpaceDesignPoints_.getSize(); ++i)
      distributions[i] = Normal(standardSpaceDesignPoints_[i], Point(inDimension, 1.));
    conditionalDistribution = Mixture(distributions);
  }
  else
    // Simple event or intersection event (single design point from min-function FORM)
    conditionalDistribution = Normal(standardSpaceDesignPoints_[0], Point(inDimension, 1.));

  const ImportanceSamplingExperiment experiment(conditionalDistribution);

  RandomVector algoEvent;
  if (getLimitState().isSystemLimitState())
  {
    // StandardEvent cannot be constructed from UnionEvent/IntersectionEvent because
    // getFunction() is not implemented for system events. Instead, manually build
    // the equivalent event in standard Normal space: compose each sub-event's function
    // with the inverse isoprobabilistic transformation, sharing a single antecedent
    // so that the UnionEvent/IntersectionEvent root-cause check passes.
    const Distribution distribution = event.getAntecedent().getDistribution();
    const Function inverse = distribution.getInverseIsoProbabilisticTransformation();
    const RandomVector sharedAntecedent(distribution.getStandardDistribution());

    Collection<RandomVector> subEvents;
    if (getLimitState().getType() == LimitState::Type::Union)
      subEvents = dynamic_cast<const UnionEvent&>(*event.getImplementation()).getEventCollection();
    else
      subEvents = dynamic_cast<const IntersectionEvent&>(*event.getImplementation()).getEventCollection();

    Collection<RandomVector> standardSubEvents(subEvents.getSize());
    for (UnsignedInteger i = 0; i < subEvents.getSize(); ++i)
    {
      const ComposedFunction composed(subEvents[i].getFunction(), inverse);
      const CompositeRandomVector vect(composed, sharedAntecedent);
      standardSubEvents[i] = ThresholdEvent(vect, subEvents[i].getOperator(), subEvents[i].getThreshold());
    }

    if (getLimitState().getType() == LimitState::Type::Union)
      algoEvent = UnionEvent(standardSubEvents);
    else
      algoEvent = IntersectionEvent(standardSubEvents);
  }
  else
  {
    algoEvent = StandardEvent(event);
  }

  SimulationInterface interface = ProbabilitySimulationAlgorithm(algoEvent, experiment);
  
  return interface;
}


Sample ImportanceSamplingAnalysis::getStandardSpaceDesignPoints() const
{
  return standardSpaceDesignPoints_;
}


void ImportanceSamplingAnalysis::setStandardSpaceDesignPoints(const Sample& points)
{
  for (UnsignedInteger i = 0; i < points.getSize(); ++i)
    if (points[i].getDimension() != getPhysicalModel().getStochasticInputNames().getSize())
      throw InvalidArgumentException(HERE) << "Design point " << i << " has dimension " << points[i].getDimension()
        << " but physical model stochastic input dimension is " << getPhysicalModel().getStochasticInputNames().getSize() << ".";
  standardSpaceDesignPoints_ = points;
}


Parameters ImportanceSamplingAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Importance sampling");
  param.add("Outputs of interest", Parameters::GetOTDescriptionStr(getLimitState().getOutputNames(), false, false));
  param.add("Design points (standard space)", Parameters::GetOTSampleStr(getStandardSpaceDesignPoints()));
  param.add(WithStopCriteriaAnalysis::getParameters());
  param.add(SimulationReliabilityAnalysis::getParameters());

  return param;
}


String ImportanceSamplingAnalysis::getPythonScript() const
{
  OSS oss;
  oss << SimulationReliabilityAnalysis::getPythonScript();
  oss << getName() << ".setStandardSpaceDesignPoints(" << Parameters::GetOTSampleStr(getStandardSpaceDesignPoints()) << ")\n";

  return oss;
}


/* String converter */
String ImportanceSamplingAnalysis::__repr__() const
{
  OSS oss;
  oss << SimulationReliabilityAnalysis::__repr__()
      << " standardSpaceDesignPoints=" << getStandardSpaceDesignPoints();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportanceSamplingAnalysis::save(Advocate & adv) const
{
  SimulationReliabilityAnalysis::save(adv);
  adv.saveAttribute("standardSpaceDesignPoints_", standardSpaceDesignPoints_);
}


/* Method load() reloads the object from the StorageManager */
void ImportanceSamplingAnalysis::load(Advocate & adv)
{
  SimulationReliabilityAnalysis::load(adv);
  if (adv.hasAttribute("standardSpaceDesignPoints_"))
    adv.loadAttribute("standardSpaceDesignPoints_", standardSpaceDesignPoints_);
  else
  {
    Point standardSpaceDesignPoint;
    adv.loadAttribute("standardSpaceDesignPoint_", standardSpaceDesignPoint);
    standardSpaceDesignPoints_ = Sample(1, standardSpaceDesignPoint);
  }
}
}
