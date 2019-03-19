//                                               -*- C++ -*-
/**
 *  @brief Analysis SORM
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
#include "otgui/SORMAnalysis.hxx"

#include <openturns/SORM.hxx>
#include <openturns/CompositeRandomVector.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(SORMAnalysis)

static Factory<SORMAnalysis> Factory_SORMAnalysis;

/* Default constructor */
SORMAnalysis::SORMAnalysis()
  : ReliabilityAnalysis()
  , ApproximationAnalysis()
{
}


/* Constructor with parameters */
SORMAnalysis::SORMAnalysis(const String& name, const LimitState& limitState)
  : ReliabilityAnalysis(name, limitState)
  , ApproximationAnalysis()
{
  setPhysicalStartingPoint(limitState.getPhysicalModel().getDistribution().getMean());
}


/* Virtual constructor */
SORMAnalysis* SORMAnalysis::clone() const
{
  return new SORMAnalysis(*this);
}


void SORMAnalysis::initialize()
{
  // clear result
  AnalysisImplementation::initialize();
  result_ = SORMAnalysisResult();
}


void SORMAnalysis::launch()
{
  const Description outputName(1, getLimitState().getOutputName());

  // get function
  Function function(getPhysicalModel().getRestrictedFunction(outputName));

  // create OT::Event
  Event event(CompositeRandomVector(function, getPhysicalModel().getInputRandomVector()), getLimitState().getOperator(), getLimitState().getThreshold());
  event.setDescription(outputName);

  optimizationAlgorithm_.setStopCallback(&AnalysisImplementation::Stop, this);

  // create OT::FORM
  SORM algo(getOptimizationAlgorithm(), event, getPhysicalStartingPoint());

  // run algo
  algo.run();

  // set result
  result_.sormResult_ = algo.getResult();

  // compute Hasofer reliability index sensitivity (not computed by default)
  result_.sormResult_.getHasoferReliabilityIndexSensitivity();
}


SORMAnalysisResult SORMAnalysis::getResult() const
{
  return result_;
}


Parameters SORMAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "SORM");
  param.add("Output of interest", getLimitState().getOutputName());
  param.add("Optimization algorithm", getOptimizationAlgorithm().getImplementation()->getClassName());
  param.add("Physical starting point", getPhysicalStartingPoint());
  param.add("Maximum evaluation number", getOptimizationAlgorithm().getMaximumEvaluationNumber());
  param.add("Maximum absolute error", getOptimizationAlgorithm().getMaximumAbsoluteError());
  param.add("Maximum relative error", getOptimizationAlgorithm().getMaximumRelativeError());
  param.add("Maximum residual error", getOptimizationAlgorithm().getMaximumResidualError());
  param.add("Maximum constraint error", getOptimizationAlgorithm().getMaximumConstraintError());

  return param;
}


String SORMAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = otguibase.SORMAnalysis('" << getName() << "', " << getLimitState().getName() << ")\n";
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


bool SORMAnalysis::hasValidResult() const
{
  return result_.getSORMResult().getStandardSpaceDesignPoint().getDimension() != 0;
}


/* String converter */
String SORMAnalysis::__repr__() const
{
  OSS oss;
  oss << ReliabilityAnalysis::__repr__()
      << ApproximationAnalysis::__repr__();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SORMAnalysis::save(Advocate & adv) const
{
  ReliabilityAnalysis::save(adv);
  ApproximationAnalysis::save(adv);
  adv.saveAttribute("sormanalysisresult_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SORMAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  ApproximationAnalysis::load(adv);
  adv.loadAttribute("sormanalysisresult_", result_);
  // can open older xml files
  if (!result_.getSORMResult().getStandardSpaceDesignPoint().getDimension())
  {
    SORMResult sormResu;
    adv.loadAttribute("result_", sormResu);
    result_.sormResult_ = sormResu;
  }
}
}
