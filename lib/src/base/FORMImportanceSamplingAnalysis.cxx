//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the FORM-Importance sampling method
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/FORMImportanceSamplingAnalysis.hxx"

#include "persalys/FORMAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FORMImportanceSamplingAnalysis)

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
  setPhysicalStartingPoint(limitState.getPhysicalModel().getDistribution().getMean());
}


/* Virtual constructor */
FORMImportanceSamplingAnalysis* FORMImportanceSamplingAnalysis::clone() const
{
  return new FORMImportanceSamplingAnalysis(*this);
}


void FORMImportanceSamplingAnalysis::initialize()
{
  ImportanceSamplingAnalysis::initialize();
  result_ = SimulationReliabilityResult();
  FORMResult_ = FORMResult();
  notify("progressValueChanged");
}


void FORMImportanceSamplingAnalysis::launch()
{
  // FORM analysis
  FORMAnalysis formAnalysis("aFORMAnalysis", getLimitState());
  optimizationAlgorithm_.setStopCallback(&AnalysisImplementation::Stop, this);
  optimizationAlgorithm_.setProgressCallback(&AnalysisImplementation::UpdateProgressValue, this);
  formAnalysis.setOptimizationAlgorithm(getOptimizationAlgorithm());
  formAnalysis.setPhysicalStartingPoint(getPhysicalStartingPoint());

  // information message
  informationMessage_ = OSS() << "The FORM analysis is running";
  notify("informationMessageUpdated");

  // launch FORM analysis
  try
  {
    formAnalysis.launch();

    if (!formAnalysis.hasValidResult())
      throw InternalException(HERE) << "FORM result empty.\n";

    // set FORM result
    FORMResult_ = formAnalysis.getResult().getFORMResult();
  }
  catch (std::exception &ex)
  {
    throw InternalException(HERE) << "Error when processing the FORM analysis.\n" << ex.what();
  }

  // if the FORM analysis has been stopped
  if (stopRequested_)
  {
    throw InternalException(HERE) << "The FORM analysis has been stopped.\n";
  }

  // Importance sampling
  setStandardSpaceDesignPoint(FORMResult_.getStandardSpaceDesignPoint());
  ImportanceSamplingAnalysis::launch();
}


FORMResult FORMImportanceSamplingAnalysis::getFORMResult() const
{
  return FORMResult_;
}


Parameters FORMImportanceSamplingAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "FORM - Importance sampling");
  param.add("Output of interest", getLimitState().getOutputName());
  param.add("Design point (standard space)", getStandardSpaceDesignPoint());
  param.add(WithStopCriteriaAnalysis::getParameters());
  param.add(SimulationReliabilityAnalysis::getParameters());

  return param;
}


String FORMImportanceSamplingAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << SimulationReliabilityAnalysis::getPythonScript();
  oss << getName() << ".setPhysicalStartingPoint(" << getPhysicalStartingPoint().__str__() << ")\n";
  // optimization algo
  oss << "optimizationAlgo = ot." << getOptimizationAlgorithm().getImplementation()->getClassName() << "()\n";
  oss << "optimizationAlgo.setMaximumEvaluationNumber(" << getOptimizationAlgorithm().getMaximumEvaluationNumber() << ")\n";
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
