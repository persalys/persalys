//                                               -*- C++ -*-
/**
 *  @brief Analysis FORM
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
#include "persalys/FORMAnalysis.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/FORM.hxx>
#include <openturns/SystemFORM.hxx>
#include <openturns/CompositeRandomVector.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FORMAnalysis)

const static Factory<FORMAnalysis> Factory_FORMAnalysis;

/* Default constructor */
FORMAnalysis::FORMAnalysis()
  : ReliabilityAnalysis()
  , ApproximationAnalysis()
{
}


/* Constructor with parameters */
FORMAnalysis::FORMAnalysis(const String& name, const LimitState& limitState, bool useISEvent)
  : ReliabilityAnalysis(name, limitState)
  , ApproximationAnalysis()
  , useISEvent_(useISEvent)
{
  setPhysicalStartingPoint(limitState.getPhysicalModel().getDistribution().getMean());
}


/* Virtual constructor */
FORMAnalysis* FORMAnalysis::clone() const
{
  return new FORMAnalysis(*this);
}


void FORMAnalysis::initialize()
{
  // clear result
  AnalysisImplementation::initialize();
  result_ = FORMAnalysisResult();
  notifyProgress();
}


void FORMAnalysis::launch()
{
  if (!getPhysicalModel().getDistribution().isContinuous())
    throw InvalidArgumentException(HERE) << "The model distribution must have continuous marginals.";

  RandomVector event;
  if(useISEvent_)
    event = getLimitState().asComposedEvent();
  else
    event = getLimitState().getThresholdEvent();

  OptimizationAlgorithm solver(getOptimizationAlgorithm());
  solver.setStopCallback(&AnalysisImplementation::Stop, this);
  solver.setProgressCallback(&UpdateProgressValue, this);
  solver.setStartingPoint(getPhysicalStartingPoint());


  if (getLimitState().isSystemLimitState() && !(useISEvent_ && getLimitState().getType() == LimitState::Type::Intersection))
  {
    // create OT::SystemFORM
    SystemFORM algo(solver, event);
    // run algo
    algo.run();
    // set result
    result_ = FORMAnalysisResult(algo.getResult());
  }
  else
  {
    // create OT::FORM
    FORM algo(solver, event);
    // run algo
    algo.run();
    // set result
    result_ = FORMAnalysisResult(algo.getResult());

    // compute event probability sensitivity (not computed by default)
    try
    {
      result_.formResult_.getEventProbabilitySensitivity();
    }
    catch (const InvalidArgumentException &)
    {
      // do nothing
      LOGWARN(" Error when computing the event probability sensitivity");
    }
  }
}


FORMAnalysisResult FORMAnalysis::getResult() const
{
  return result_;
}


Parameters FORMAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "FORM");
  param.add("Output of interest", Parameters::GetOTDescriptionStr(getLimitState().getOutputNames(), false, false));
  param.add("Optimization algorithm", getOptimizationAlgorithm().getImplementation()->getClassName());
  param.add("Physical starting point", getPhysicalStartingPoint());
  param.add("Maximum number of calls", getOptimizationAlgorithm().getMaximumCallsNumber());
  param.add("Maximum absolute error", getOptimizationAlgorithm().getMaximumAbsoluteError());
  param.add("Maximum relative error", getOptimizationAlgorithm().getMaximumRelativeError());
  param.add("Maximum residual error", getOptimizationAlgorithm().getMaximumResidualError());
  param.add("Maximum constraint error", getOptimizationAlgorithm().getMaximumConstraintError());

  return param;
}


String FORMAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = persalys.FORMAnalysis('" << getName() << "', " << getLimitState().getName() << ")\n";
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


bool FORMAnalysis::hasValidResult() const
{
  bool hasResult = false;

  try {
    if (getLimitState().isSystemLimitState() && !(useISEvent_ && getLimitState().getType() == LimitState::Type::Intersection))
      hasResult = result_.getMultiFORMResult().getFORMResultCollection().getSize() != 0 
                  && result_.getMultiFORMResult().getFORMResultCollection()[0].getStandardSpaceDesignPoint().getDimension() != 0;
    else
      hasResult = result_.getFORMResult().getStandardSpaceDesignPoint().getDimension() != 0;
  }
  catch (const InvalidArgumentException &)
  {
    // return false
  }
  
  return hasResult;
}


/* String converter */
String FORMAnalysis::__repr__() const
{
  OSS oss;
  oss << ReliabilityAnalysis::__repr__()
      << ApproximationAnalysis::__repr__();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FORMAnalysis::save(Advocate & adv) const
{
  ReliabilityAnalysis::save(adv);
  ApproximationAnalysis::save(adv);
  adv.saveAttribute("formanalysisresult_", result_);
  adv.saveAttribute("useISEvent_", useISEvent_);
}


/* Method load() reloads the object from the StorageManager */
void FORMAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  ApproximationAnalysis::load(adv);
  adv.loadAttribute("formanalysisresult_", result_);
  // can open older xml files
  if (adv.hasAttribute("result_") && !result_.getFORMResult().getStandardSpaceDesignPoint().getDimension())
  {
    FORMResult formResu;
    adv.loadAttribute("result_", formResu);
    result_ = FORMAnalysisResult(formResu);
  }
  if (adv.hasAttribute("useISEvent_"))
    adv.loadAttribute("useISEvent_", useISEvent_);
}
}
