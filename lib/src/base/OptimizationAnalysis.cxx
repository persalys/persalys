//                                               -*- C++ -*-
/**
 *  @brief Optimization of a physical model
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
#include "persalys/OptimizationAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/OptimizationAlgorithm.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/SymbolicFunction.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(OptimizationAnalysis)

static Factory<OptimizationAnalysis> Factory_OptimizationAnalysis;

Description OptimizationAnalysis::SolverNames_;

Description OptimizationAnalysis::GetSolverNames()
{
  // exclude least-squares or nearest-point
  OptimizationProblem problem(SymbolicFunction(Description(1, "x"), Description(1, "x^2")));
  return OptimizationAlgorithm::GetAlgorithmNames(problem);
}

/* Default constructor */
OptimizationAnalysis::OptimizationAnalysis()
  : PhysicalModelAnalysis()
  , solverName_("Cobyla")
  , isMinimization_(true)
  , startingPoint_()
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError"))
  , bounds_()
  , result_()
{
}


/* Constructor with parameters */
OptimizationAnalysis::OptimizationAnalysis(const String& name,
    const PhysicalModel& physicalModel,
    const String& algorithmName)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , solverName_(algorithmName)
  , isMinimization_(true)
  , startingPoint_()
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError"))
  , bounds_()
  , variableInputs_()
  , result_()
{
  initializeParameters();
  if (getPhysicalModel().getSelectedOutputsNames().getSize() > 0)
    setInterestVariables(Description(1, getPhysicalModel().getSelectedOutputsNames()[0]));
}


/* Virtual constructor */
OptimizationAnalysis* OptimizationAnalysis::clone() const
{
  return new OptimizationAnalysis(*this);
}


void OptimizationAnalysis::setInterestVariables(const Description& variablesNames)
{
  if (variablesNames.getSize() > 1)
    throw InvalidArgumentException(HERE) << "Define only one variable of interest";
  AnalysisImplementation::setInterestVariables(variablesNames);
}


void OptimizationAnalysis::initializeParameters()
{
  inputNames_ = getPhysicalModel().getInputNames();
  variableInputs_ = inputNames_;

  Tools::ComputeBounds(getPhysicalModel().getInputs(), startingPoint_, bounds_);
}


void OptimizationAnalysis::updateParameters()
{
  const Description inputNames(inputNames_);
  const Point startingPoint(startingPoint_);
  const Point lowerBounds(bounds_.getLowerBound());
  const Point upperBounds(bounds_.getUpperBound());
  const Interval::BoolCollection finiteLowerBounds(bounds_.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBounds(bounds_.getFiniteUpperBound());
  const Description variableInputs(variableInputs_);

  initializeParameters();

  Point newLowerBounds(bounds_.getLowerBound());
  Point newUpperBounds(bounds_.getUpperBound());
  Interval::BoolCollection newFiniteLowerBounds(bounds_.getFiniteLowerBound());
  Interval::BoolCollection newFiniteUpperBounds(bounds_.getFiniteUpperBound());

  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      startingPoint_[i] = startingPoint[it - inputNames.begin()];
      newLowerBounds[i] = lowerBounds[it - inputNames.begin()];
      newUpperBounds[i] = upperBounds[it - inputNames.begin()];
      newFiniteLowerBounds[i] = finiteLowerBounds[it - inputNames.begin()];
      newFiniteUpperBounds[i] = finiteUpperBounds[it - inputNames.begin()];
      if (!variableInputs.contains(inputNames_[i]))
        variableInputs_.erase(std::remove(variableInputs_.begin(), variableInputs_.end(), inputNames_[i]), variableInputs_.end());
    }
  }
  bounds_.setLowerBound(newLowerBounds);
  bounds_.setUpperBound(newUpperBounds);
  bounds_.setFiniteLowerBound(newFiniteLowerBounds);
  bounds_.setFiniteUpperBound(newFiniteUpperBounds);
}


void OptimizationAnalysis::initialize()
{
  PhysicalModelAnalysis::initialize();
  result_ = OptimizationResult();
  notify("progressValueChanged");
}


void OptimizationAnalysis::launch()
{
  const Description modelInputNames(getPhysicalModel().getInputNames());
  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();
  // check
  if (getInterestVariables().getSize() != 1)
    throw InvalidArgumentException(HERE) << "Specify one variable to optimize";
  if (startingPoint_.getDimension() != nbInputs)
    throw InvalidArgumentException(HERE) << "Invalid starting point";
  if (!variableInputs_.getSize())
    throw InvalidArgumentException(HERE) << "At least one variable must vary";
  Description allVar(variableInputs_);
  allVar.add(inputNames_);
  for (UnsignedInteger i = 0; i < allVar.getSize(); ++i)
  {
    if (!getPhysicalModel().hasInputNamed(allVar[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an input variable named " << allVar[i];
  }

  // get bounds and fixed inputs values
  if (getBounds().getDimension() != nbInputs)
    throw InvalidArgumentException(HERE) << "The interval's dimension must be equal to the number of model's inputs";
  Indices fixedInputsIndices;
  Point fixedInputsValues;
  Point variableInputsValues;
  Indices variableInputsIndices;
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    if (!variableInputs_.contains(modelInputNames[i]))
    {
      fixedInputsIndices.add(i);
      fixedInputsValues.add(startingPoint_[i]);
    }
    else
    {
      variableInputsIndices.add(i);
      variableInputsValues.add(startingPoint_[i]);
    }
  }
  // check bounds
  if (bounds_.getMarginal(variableInputsIndices).isEmpty())
    throw InvalidArgumentException(HERE) << "The lower bounds must be less than the upper bounds";

  // set objective
  Function objective = getPhysicalModel().getFunction(getInterestVariables());
  if (fixedInputsIndices.getSize())
    objective = ParametricFunction(objective, fixedInputsIndices, fixedInputsValues);

  // set OptimizationProblem
  OptimizationProblem problem(objective, Function(), Function(), bounds_.getMarginal(variableInputsIndices));
  problem.setMinimization(isMinimization_);

  // build solver
  OptimizationAlgorithm solver(OptimizationAlgorithm::Build(solverName_));
  solver.setProblem(problem);
  solver.setStartingPoint(variableInputsValues);
  solver.setMaximumEvaluationNumber(getMaximumEvaluationNumber());
  solver.setMaximumAbsoluteError(getMaximumAbsoluteError());
  solver.setMaximumRelativeError(getMaximumRelativeError());
  solver.setMaximumResidualError(getMaximumResidualError());
  solver.setMaximumConstraintError(getMaximumConstraintError());
  solver.setStopCallback(&AnalysisImplementation::Stop, this);
  solver.setProgressCallback(&UpdateProgressValue, this);

  // run solver
  solver.run();

  // set result
  result_ = solver.getResult();

  // FIXME: some algos forget to set the problem, fixed in 1.17
  result_.setProblem(problem);
}


String OptimizationAnalysis::getSolverName() const
{
  return solverName_;
}


void OptimizationAnalysis::setSolverName(const String& name)
{
  if (!GetSolverNames().contains(name))
    throw InvalidArgumentException(HERE) << "Error: the given solver name=" << name << " is unknown. " << GetSolverNames();

  solverName_ = name;
}


void OptimizationAnalysis::setMinimization(const Bool minimization)
{
  isMinimization_ = minimization;
}


Bool OptimizationAnalysis::getMinimization() const
{
  return isMinimization_;
}


void OptimizationAnalysis::setBounds(const Interval & bounds)
{
  if (bounds.getDimension() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "The interval's dimension must be equal to the number of model's inputs";
  bounds_ = bounds;
}


Interval OptimizationAnalysis::getBounds() const
{
  return bounds_;
}


Point OptimizationAnalysis::getStartingPoint() const
{
  return startingPoint_;
}


void OptimizationAnalysis::setStartingPoint(const Point & startingPoint)
{
  if (startingPoint.getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "The point's dimension must be equal to the number of model's inputs";
  startingPoint_ = startingPoint;
}


/* Maximum iterations number accessor */
UnsignedInteger OptimizationAnalysis::getMaximumEvaluationNumber() const
{
  return maximumEvaluationNumber_;
}


/* Maximum iterations number accessor */
void OptimizationAnalysis::setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber)
{
  maximumEvaluationNumber_ = maximumEvaluationNumber;
}


/* Maximum absolute error accessor */
Scalar OptimizationAnalysis::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}


/* Maximum absolute error accessor */
void OptimizationAnalysis::setMaximumAbsoluteError(const Scalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}


/* Maximum relative error accessor */
Scalar OptimizationAnalysis::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}


/* Maximum relative error accessor */
void OptimizationAnalysis::setMaximumRelativeError(const Scalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}


/* Maximum residual error accessor */
Scalar OptimizationAnalysis::getMaximumResidualError() const
{
  return maximumResidualError_;
}


/* Maximum residual error accessor */
void OptimizationAnalysis::setMaximumResidualError(const Scalar maximumResidualError)
{
  maximumResidualError_ = maximumResidualError;
}


/* Maximum constraint error accessor */
Scalar OptimizationAnalysis::getMaximumConstraintError() const
{
  return maximumConstraintError_;
}


/* Maximum constraint error accessor */
void OptimizationAnalysis::setMaximumConstraintError(const Scalar maximumConstraintError)
{
  maximumConstraintError_ = maximumConstraintError;
}


Description OptimizationAnalysis::getVariableInputs() const
{
  return variableInputs_;
}


void OptimizationAnalysis::setVariableInputs(const Description& inputs)
{
  for (UnsignedInteger i = 0; i < inputs.getSize(); ++i)
  {
    if (!getPhysicalModel().hasInputNamed(inputs[i]))
      throw InvalidArgumentException(HERE) << "Error: the physical model does not contain an input variable named " << inputs[i];
  }

  variableInputs_ = inputs;
}


OptimizationResult OptimizationAnalysis::getResult() const
{
  return result_;
}


Parameters OptimizationAnalysis::getParameters() const
{
  Parameters param;

  param.add("Analysis", "Optimization");
  param.add("Output of interest", getInterestVariables().__str__());
  param.add("Solver", getSolverName());
  param.add("Optimization type", getMinimization() ? "Minimization" : "Maximization");
  param.add("Maximum number of function evaluations", getMaximumEvaluationNumber());
  param.add("Maximum absolute error", getMaximumAbsoluteError());
  param.add("Maximum relative error", getMaximumRelativeError());
  param.add("Maximum residual error", getMaximumResidualError());
  param.add("Maximum constraint error", getMaximumConstraintError());

  Description allInputsBoundsStr = Parameters::GetOTIntervalDescription(getBounds());
  Description fixedInputs;
  String varInputs;
  Point startingPoint;
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
  {
    if (!variableInputs_.contains(inputNames_[i]))
    {
      fixedInputs.add(OSS() << inputNames_[i] << " : " << startingPoint_[i]);
    }
    else
    {
      varInputs += inputNames_[i] + " : " + allInputsBoundsStr[i];
      startingPoint.add(startingPoint_[i]);
      if (startingPoint.getSize() <  variableInputs_.getSize())
        varInputs += "\n";
    }
  }
  param.add("Starting point", startingPoint);
  param.add("Bounds", varInputs);
  if (fixedInputs.getSize())
    param.add("Fixed inputs", fixedInputs.__str__());

  return param;
}


String OptimizationAnalysis::getPythonScript() const
{
  String result;

  OSS oss;

  oss << getName() << " = persalys.OptimizationAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ", '" << solverName_ << "')\n";
  // interest output
  if (getInterestVariables().getSize() < getPhysicalModel().getOutputNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  oss << getName() << ".setMinimization(" << (isMinimization_ ? "True" : "False") << ")\n";
  oss << "bounds = ot.Interval(" << getBounds().getLowerBound().__str__()
      << ", " << getBounds().getUpperBound().__str__()
      << ", " << Parameters::GetOTBoolCollectionStr(getBounds().getFiniteLowerBound())
      << ", " << Parameters::GetOTBoolCollectionStr(getBounds().getFiniteUpperBound());
  oss << ")\n";

  oss << getName() << ".setBounds(bounds)\n";
  oss << getName() << ".setStartingPoint(" << getStartingPoint().__str__() << ")\n";
  oss << getName() << ".setVariableInputs(" << Parameters::GetOTDescriptionStr(getVariableInputs()) << ")\n";

  oss << getName() << ".setMaximumEvaluationNumber(" << getMaximumEvaluationNumber() << ")\n";
  oss << getName() << ".setMaximumAbsoluteError(" << getMaximumAbsoluteError() << ")\n";
  oss << getName() << ".setMaximumRelativeError(" << getMaximumRelativeError() << ")\n";
  oss << getName() << ".setMaximumResidualError(" << getMaximumResidualError() << ")\n";
  oss << getName() << ".setMaximumConstraintError(" << getMaximumConstraintError() << ")\n";

  return oss;
}


bool OptimizationAnalysis::hasValidResult() const
{
  return result_.getOptimalPoint().getSize() != 0;
}


/* String converter */
String OptimizationAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " algorithmName=" << getSolverName()
      << " isMinimization=" << getMinimization()
      << " starting point=" << getStartingPoint()
      << " maximumEvaluationNumber=" << getMaximumEvaluationNumber()
      << " maximumAbsoluteError=" << getMaximumAbsoluteError()
      << " maximumRelativeError=" << getMaximumRelativeError()
      << " maximumResidualError=" << getMaximumResidualError()
      << " maximumConstraintError=" << getMaximumConstraintError()
      << " bounds=" << Parameters::GetOTIntervalDescription(getBounds())
      << " variable inputs=" << getVariableInputs();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void OptimizationAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("solverName_", solverName_);
  adv.saveAttribute("isMinimization_", isMinimization_);
  adv.saveAttribute("startingPoint_", startingPoint_);
  adv.saveAttribute("maximumEvaluationNumber_", maximumEvaluationNumber_);
  adv.saveAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.saveAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.saveAttribute("maximumResidualError_", maximumResidualError_);
  adv.saveAttribute("maximumConstraintError_", maximumConstraintError_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("variableInputs_", variableInputs_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void OptimizationAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("solverName_", solverName_);
  adv.loadAttribute("isMinimization_", isMinimization_);
  adv.loadAttribute("startingPoint_", startingPoint_);
  adv.loadAttribute("maximumEvaluationNumber_", maximumEvaluationNumber_);
  adv.loadAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.loadAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.loadAttribute("maximumResidualError_", maximumResidualError_);
  adv.loadAttribute("maximumConstraintError_", maximumConstraintError_);
  adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("variableInputs_", variableInputs_);
  adv.loadAttribute("result_", result_);
}
}
