//                                               -*- C++ -*-
/**
 *  @brief Analysis FORM
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
#include "otgui/FORMAnalysis.hxx"

#include <openturns/FORM.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(FORMAnalysis)

static Factory<FORMAnalysis> Factory_FORMAnalysis;

/* Default constructor */
FORMAnalysis::FORMAnalysis()
  : ReliabilityAnalysis()
  , ApproximationAnalysis()
{
}


/* Constructor with parameters */
FORMAnalysis::FORMAnalysis(const String& name, const LimitState& limitState)
  : ReliabilityAnalysis(name, limitState)
  , ApproximationAnalysis()
{
  setPhysicalStartingPoint(limitState.getPhysicalModel().getComposedDistribution().getMean());
}


/* Virtual constructor */
FORMAnalysis* FORMAnalysis::clone() const
{
  return new FORMAnalysis(*this);
}


void FORMAnalysis::run()
{
  isRunning_ = true;
  try
  {
    // clear result
    initialize();
    result_ = FORMResult();

    Description outputName(1);
    outputName[0] = getLimitState().getOutputName();

    // get function
    Function function(getPhysicalModel().getRestrictedFunction(outputName));

    // create OT::Event
    Event event(RandomVector(function, getPhysicalModel().getInputRandomVector()), getLimitState().getOperator(), getLimitState().getThreshold());
    event.setDescription(outputName);

    // create OT::FORM
    FORM algo(getOptimizationAlgorithm(), event, getPhysicalStartingPoint());

    // run algo
    algo.run();

    // set result
    result_ = algo.getResult();

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
  isRunning_ = false;
}


FORMResult FORMAnalysis::getResult() const
{
  return result_;
}


String FORMAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = otguibase.FORMAnalysis('" << getName() << "', " << getLimitState().getName() << ")\n";
  oss << getName() << ".setPhysicalStartingPoint(" << getPhysicalStartingPoint().__str__() << ")\n";
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


bool FORMAnalysis::analysisLaunched() const
{
  return result_.getStandardSpaceDesignPoint().getDimension() != 0;
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
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void FORMAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  ApproximationAnalysis::load(adv);
  adv.loadAttribute("result_", result_);
}
}
