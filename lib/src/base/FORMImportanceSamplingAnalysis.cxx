//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the FORM-Importance sampling method
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
#include "otgui/FORMImportanceSamplingAnalysis.hxx"

#include "otgui/FORMAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FORMImportanceSamplingAnalysis);

static Factory<FORMImportanceSamplingAnalysis> Factory_FORMImportanceSamplingAnalysis;

/* Default constructor */
FORMImportanceSamplingAnalysis::FORMImportanceSamplingAnalysis()
  : ImportanceSamplingAnalysis()
  , ApproximationAnalysis()
{
}


/* Constructor with parameters */
FORMImportanceSamplingAnalysis::FORMImportanceSamplingAnalysis(const String& name,
                                                               const LimitState& limitState)
  : ImportanceSamplingAnalysis(name, limitState)
  , ApproximationAnalysis()
{
  setPhysicalStartingPoint(limitState.getPhysicalModel().getComposedDistribution().getMean());
}


/* Virtual constructor */
FORMImportanceSamplingAnalysis* FORMImportanceSamplingAnalysis::clone() const
{
  return new FORMImportanceSamplingAnalysis(*this);
}


void FORMImportanceSamplingAnalysis::run()
{
  isRunning_ = true;
  // No section try/except here as for the other analyses
  // because this section is directly in FORMAnalysis/IS::run

  // clear result
  initialize();
  result_ = SimulationReliabilityResult();

  // FORM analysis
  FORMAnalysis formAnalysis("aFORMAnalysis", getLimitState());
  formAnalysis.setOptimizationAlgorithm(getOptimizationAlgorithm());
  formAnalysis.setPhysicalStartingPoint(getPhysicalStartingPoint());
  formAnalysis.run();

  const Point designPoint(formAnalysis.getResult().getStandardSpaceDesignPoint());
  if (!formAnalysis.getErrorMessage().empty() || designPoint.getSize() == 0)
  {
    errorMessage_ = "Error when processing the FORM analysis.\n" + formAnalysis.getErrorMessage();
    isRunning_ = false;
    notify("analysisBadlyFinished");
    return;
  }

  // set FORM result
  FORMResult_ = formAnalysis.getResult();

  // Importance sampling
  setStandardSpaceDesignPoint(designPoint);
  ImportanceSamplingAnalysis::run();
  isRunning_ = false;
}


FORMResult FORMImportanceSamplingAnalysis::getFORMResult() const
{
  return FORMResult_;
}


String FORMImportanceSamplingAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << ImportanceSamplingAnalysis::getPythonScript();
  oss << getName() << ".setPhysicalStartingPoint(" << getPhysicalStartingPoint().__str__() << ")\n";
  oss << getName() << ".setStandardSpaceDesignPoint(" << getStandardSpaceDesignPoint().__str__() << ")\n";
  // optimization algo
  oss << "optimizationAlgo = ot." << getOptimizationAlgorithm().getImplementation()->getClassName() << "()\n";
  oss << "optimizationAlgo.setMaximumIterationNumber(" << getOptimizationAlgorithm().getMaximumIterationNumber() << ")\n";
  oss << "optimizationAlgo.setMaximumAbsoluteError(" << getOptimizationAlgorithm().getMaximumAbsoluteError() << ")\n";
  oss << "optimizationAlgo.setMaximumRelativeError(" << getOptimizationAlgorithm().getMaximumRelativeError() << ")\n";
  oss << "optimizationAlgo.setMaximumResidualError(" << getOptimizationAlgorithm().getMaximumResidualError() << ")\n";
  oss << "optimizationAlgo.setMaximumConstraintError(" << getOptimizationAlgorithm().getMaximumConstraintError() << ")\n";
  oss << getName() << ".setOptimizationAlgorithm(optimizationAlgo)\n";

  return oss;
}


/* String converter */
String FORMImportanceSamplingAnalysis::__repr__() const
{
  OSS oss;
  oss << ImportanceSamplingAnalysis::__repr__()
      << ApproximationAnalysis::__repr__();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FORMImportanceSamplingAnalysis::save(Advocate & adv) const
{
  ImportanceSamplingAnalysis::save(adv);
  ApproximationAnalysis::save(adv);
  adv.saveAttribute("FORMResult_", FORMResult_);
}


/* Method load() reloads the object from the StorageManager */
void FORMImportanceSamplingAnalysis::load(Advocate & adv)
{
  ImportanceSamplingAnalysis::load(adv);
  ApproximationAnalysis::load(adv);
  adv.loadAttribute("FORMResult_", FORMResult_);
}
}
