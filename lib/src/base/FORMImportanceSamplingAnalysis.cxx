//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the FORM-Importance sampling method
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
#include "persalys/FORMImportanceSamplingAnalysis.hxx"

#include "persalys/FORMAnalysis.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FORMImportanceSamplingAnalysis)

const static Factory<FORMImportanceSamplingAnalysis> Factory_FORMImportanceSamplingAnalysis;

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
  notifyProgress();
}

void FORMImportanceSamplingAnalysis::launch()
{
  // FORM analysis
  FORMAnalysis formAnalysis("aFORMAnalysis", getLimitState(), true);
  optimizationAlgorithm_.setStopCallback(&AnalysisImplementation::Stop, this);
  optimizationAlgorithm_.setProgressCallback(&AnalysisImplementation::UpdateProgressValue, this);
  formAnalysis.setOptimizationAlgorithm(getOptimizationAlgorithm());
  formAnalysis.setPhysicalStartingPoint(getPhysicalStartingPoint());

  // information message
  informationMessage_ = "The FORM analysis is running";
  notifyMessageUpdated();

  // launch FORM analysis
  try
  {
    formAnalysis.launch();

    if (!formAnalysis.hasValidResult())
      throw InternalException(HERE) << "FORM result empty.\n";

    // set FORM result
    if (getLimitState().isSystemLimitState() && getLimitState().getType() != LimitState::Type::Intersection)
      multiFORMResult_ = formAnalysis.getResult().getMultiFORMResult();
    else
       FORMResult_ = formAnalysis.getResult().getFORMResult();
  }
  catch (const std::exception &ex)
  {
    throw InternalException(HERE) << "Error when processing the FORM analysis.\n" << ex.what();
  }

  // if the FORM analysis has been stopped
  if (stopRequested_)
  {
    throw InternalException(HERE) << "The FORM analysis has been stopped.\n";
  }

  // Importance sampling
  if (!FORMResult_.getStandardSpaceDesignPoint().isEmpty())
    setStandardSpaceDesignPoints(Sample(1, FORMResult_.getStandardSpaceDesignPoint()));
  else
  {
    const Collection<FORMResult> formResults = multiFORMResult_.getFORMResultCollection();
    Sample designPoints(formResults.getSize(), formResults[0].getStandardSpaceDesignPoint().getDimension());
    for (UnsignedInteger i = 0; i < formResults.getSize(); ++i)
      designPoints[i] = formResults[i].getStandardSpaceDesignPoint();
    setStandardSpaceDesignPoints(designPoints);
  }
  ImportanceSamplingAnalysis::launch();
}

FORMResult FORMImportanceSamplingAnalysis::getFORMResult() const
{
  return FORMResult_;
}

MultiFORMResult FORMImportanceSamplingAnalysis::getMultiFORMResult() const
{
  return multiFORMResult_;
}

Parameters FORMImportanceSamplingAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "FORM - Importance sampling");
  param.add("Outputs of interest", Parameters::GetOTDescriptionStr(getLimitState().getOutputNames(), false, false));
  param.add("Design points (standard space)", Parameters::GetOTSampleStr(getStandardSpaceDesignPoints()));
  param.add(WithStopCriteriaAnalysis::getParameters());
  param.add(SimulationReliabilityAnalysis::getParameters());

  return param;
}


String FORMImportanceSamplingAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << SimulationReliabilityAnalysis::getPythonScript();
  oss << getName() << ".setPhysicalStartingPoint(" << Parameters::GetOTPointStr(getPhysicalStartingPoint()) << ")\n";
  // optimization algo
  oss << "optimizationAlgo = ot." << getOptimizationAlgorithm().getImplementation()->getClassName() << "()\n";
  oss << "optimizationAlgo.setMaximumCallsNumber(" << getOptimizationAlgorithm().getMaximumCallsNumber() << ")\n";
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
  adv.saveAttribute("multiFORMResult_", multiFORMResult_);
}


/* Method load() reloads the object from the StorageManager */
void FORMImportanceSamplingAnalysis::load(Advocate & adv)
{
  ImportanceSamplingAnalysis::load(adv);
  ApproximationAnalysis::load(adv);
  adv.loadAttribute("FORMResult_", FORMResult_);
  if (adv.hasAttribute("multiFORMResult_"))
    adv.loadAttribute("multiFORMResult_", multiFORMResult_);
}
}
