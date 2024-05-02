//                                               -*- C++ -*-
/**
 *  @brief Optimization of a physical model
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include <openturns/AggregatedFunction.hxx>
#include <openturns/ComposedFunction.hxx>
#include <openturns/Pagmo.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(OptimizationAnalysis)

static Factory<OptimizationAnalysis> Factory_OptimizationAnalysis;
typedef AlgorithmProperty::Locality Locality;
typedef AlgorithmProperty::Derivative Derivative;
typedef AlgorithmProperty::Priority Priority;

std::map<OT::String, AlgorithmProperty> OptimizationAnalysis::AlgorithmDictionary =
{
  {"AUGLAG",                       AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"AUGLAG_EQ",                    AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GD_MLSL",                      AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"), Priority::Medium)},
  {"GD_MLSL_LDS",                  AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GD_STOGO",                     AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GD_STOGO_RAND",                AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_AGS",                       AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_CRS2_LM",                   AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_DIRECT",                    AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_DIRECT_L",                  AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_DIRECT_L_NOSCAL",           AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_DIRECT_L_RAND",             AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_DIRECT_L_RAND_NOSCAL",      AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_DIRECT_NOSCAL",             AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_ESCH",                      AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_ISRES",                     AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_MLSL",                      AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"), Priority::Medium)},
  {"GN_MLSL_LDS",                  AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_ORIG_DIRECT",               AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"GN_ORIG_DIRECT_L",             AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"G_MLSL",                       AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"G_MLSL_LDS",                   AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_AUGLAG",                    AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_AUGLAG_EQ",                 AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_CCSAQ",                     AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_LBFGS",                     AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"), Priority::Medium)},
  {"LD_MMA",                       AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_SLSQP",                     AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_TNEWTON",                   AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_TNEWTON_PRECOND",           AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_TNEWTON_PRECOND_RESTART",   AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_TNEWTON_RESTART",           AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_VAR1",                      AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LD_VAR2",                      AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_AUGLAG",                    AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_AUGLAG_EQ",                 AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_BOBYQA",                    AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_COBYLA",                    AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_NELDERMEAD",                AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_NEWUOA",                    AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_NEWUOA_BOUND",              AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_PRAXIS",                    AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"LN_SBPLX",                     AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.NLopt.html"))},
  {"B-BB",                         AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Bonmin.html"))},
  {"B-OA",                         AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Bonmin.html"))},
  {"B-QG",                         AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Bonmin.html"))},
  {"B-Hyb",                        AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Bonmin.html"))},
  {"B-Ecp",                        AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Bonmin.html"))},
  {"B-iFP",                        AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Bonmin.html"))},
  {"Ipopt",                        AlgorithmProperty(Locality::Local,  Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ipopt.html"))},
  {"LEVENBERG_MARQUARD",           AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ceres.html"))},
  {"DOGLEG",                       AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ceres.html"))},
  {"STEEPEST_DESCENT",             AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ceres.html"))},
  {"NONLINEAR_CONJUGATE_GRADIENT", AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ceres.html"))},
  {"LBFGS",                        AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ceres.html"), Priority::Medium)},
  {"BFGS",                         AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Ceres.html"))},
  {"Cobyla",                       AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Cobyla.html"), Priority::High)},
  {"lbfgs",                        AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"), Priority::Medium)},
  {"bfgs",                         AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"global",                       AlgorithmProperty(Locality::Global, Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"cg",                           AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"newton",                       AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"least_squares",                AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"least_squares_lm",             AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"trust_region",                 AlgorithmProperty(Locality::Global, Derivative::First, OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.Dlib.html"))},
  {"TNC",                          AlgorithmProperty(Locality::Local,  Derivative::None,  OT::String("http://openturns.github.io/openturns/latest/user_manual/_generated/openturns.TNC.html"))}
};

Description OptimizationAnalysis::GetSolverNames()
{
  // exclude least-squares or nearest-point
  OptimizationProblem problem(SymbolicFunction(Description(1, "x"), Description(1, "x^2")));
  return OptimizationAlgorithm::GetAlgorithmNames(problem);
}

Description OptimizationAnalysis::GetSolverNames(const Interval& bounds,
    const Indices& types,
    const Function& eqFunc,
    const Function& ineqFunc)
{
  // Dummy non-linear function respecting bounds dimension
  Function func = SymbolicFunction(Description::BuildDefault(bounds.getDimension(), "x"),
                                   Description(1, "x0^2"));
  OptimizationProblem problem(func, Function(), Function(), bounds);
  if (types.getSize())
    problem.setVariablesType(types);
  if (eqFunc.getInputDimension())
    problem.setEqualityConstraint(func);
  if (ineqFunc.getInputDimension())
    problem.setInequalityConstraint(func);
  Description names(OptimizationAlgorithm::GetAlgorithmNames(problem));

  // drop pagmo algos as they are parametrized by a sample instead of a single point
  const Description pagmoNames(Pagmo::GetAlgorithmNames());
  for (UnsignedInteger i = 0; i < pagmoNames.getSize(); ++ i)
  {
    const UnsignedInteger index = names.find(pagmoNames[i]);
    if (index < names.getSize())
      names.erase(names.begin() + index);
  }
  return names;
}

/* Default constructor */
OptimizationAnalysis::OptimizationAnalysis()
  : PhysicalModelAnalysis()
  , solverName_("Cobyla")
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumCallsNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError"))
{
}


/* Constructor with parameters */
OptimizationAnalysis::OptimizationAnalysis(const String& name,
    const PhysicalModel& physicalModel,
    const String& algorithmName)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , solverName_(algorithmName)
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumCallsNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError"))
{
  initializeParameters();
  variablesType_ = Indices(inputNames_.getSize(), OptimizationProblemImplementation::CONTINUOUS);
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
  const Indices types(variablesType_);

  initializeParameters();

  Point newLowerBounds(bounds_.getLowerBound());
  Point newUpperBounds(bounds_.getUpperBound());
  Indices newTypes(inputNames_.getSize());
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
      newTypes[i] = types[it - inputNames.begin()];
      if (!variableInputs.contains(inputNames_[i]))
        variableInputs_.erase(std::remove(variableInputs_.begin(), variableInputs_.end(), inputNames_[i]), variableInputs_.end());
    }
  }
  bounds_.setLowerBound(newLowerBounds);
  bounds_.setUpperBound(newUpperBounds);
  bounds_.setFiniteLowerBound(newFiniteLowerBounds);
  bounds_.setFiniteUpperBound(newFiniteUpperBounds);
  variablesType_ = newTypes;
}


void OptimizationAnalysis::initialize()
{
  PhysicalModelAnalysis::initialize();
  result_ = OptimizationResult();
  notify("progressValueChanged");
}


void OptimizationAnalysis::launch()
{
  // check
  if (getInterestVariables().getSize() != 1)
    throw InvalidArgumentException(HERE) << "Specify one variable to optimize";
  if (startingPoint_.getDimension() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "Invalid starting point";
  if (!variableInputs_.getSize())
    throw InvalidArgumentException(HERE) << "At least one variable must vary";

  OptimizationProblem problem = defineProblem();
  problem.setMinimization(isMinimization_);

  // build solver
  OptimizationAlgorithm solver(OptimizationAlgorithm::Build(solverName_));
  solver.setProblem(problem);
  solver.setStartingPoint(variableInputsValues_);
  solver.setMaximumCallsNumber(getMaximumCallsNumber());
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
}

OptimizationProblem OptimizationAnalysis::defineProblem()
{
  const Description modelInputNames(getPhysicalModel().getInputNames());
  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();

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
  Indices variablesType;
  variableInputsIndices_ = Indices();
  variableInputsValues_ = Point();
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    if (!variableInputs_.contains(modelInputNames[i]))
    {
      fixedInputsIndices.add(i);
      fixedInputsValues.add(startingPoint_[i]);
    }
    else
    {
      variableInputsIndices_.add(i);
      variablesType.add(variablesType_[i]);
      variableInputsValues_.add(startingPoint_[i]);
    }
  }

  // check bounds
  if (bounds_.getMarginal(variableInputsIndices_).isEmpty())
    throw InvalidArgumentException(HERE) << "The lower bounds must be less than the upper bounds";

  // set objective
  Function objective = getPhysicalModel().getFunction(getInterestVariables());
  Function equalityConstraints = getEqualityConstraints();
  Function inequalityConstraints = getInequalityConstraints();
  if (fixedInputsIndices.getSize())
  {
    objective = ParametricFunction(objective, fixedInputsIndices, fixedInputsValues);
    if (equalityConstraints.getInputDimension())
      equalityConstraints = ParametricFunction(equalityConstraints, fixedInputsIndices, fixedInputsValues);
    if (inequalityConstraints.getInputDimension())
      inequalityConstraints = ParametricFunction(inequalityConstraints, fixedInputsIndices, fixedInputsValues);
  }

  // set OptimizationProblem
  OptimizationProblem problem(objective, equalityConstraints, inequalityConstraints, bounds_.getMarginal(variableInputsIndices_));
  problem.setVariablesType(variablesType);
  return problem;

}

String OptimizationAnalysis::getSolverName() const
{
  return solverName_;
}


void OptimizationAnalysis::setSolverName(const String& name)
{
  if (!GetSolverNames(getBounds()).contains(name))
    throw InvalidArgumentException(HERE) << "Error: the given solver name=" << name << " is unknown. " << GetSolverNames(getBounds());
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


void OptimizationAnalysis::setVariablesType(const Indices& variablesType)
{
  if (variablesType.getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "The size of variables type must be equal to the number of model's inputs";
  variablesType_ = variablesType;
}


Indices OptimizationAnalysis::getVariablesType() const
{
  return variablesType_;
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
UnsignedInteger OptimizationAnalysis::getMaximumCallsNumber() const
{
  return maximumEvaluationNumber_;
}


/* Maximum iterations number accessor */
void OptimizationAnalysis::setMaximumCallsNumber(const UnsignedInteger maximumEvaluationNumber)
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

void OptimizationAnalysis::addConstraint(const String& equation)
{
  std::regex equationStr("(.*)([<>=])(.*)");
  std::smatch match;
  std::regex_search(equation, match, equationStr);
  if (match.size() != 4)
    throw InvalidArgumentException(HERE) << "Error in adding equation: " << equation;
  addConstraint(match[1], match[2], match[3]);
}

void OptimizationAnalysis::addConstraint(const String& leftPart,
    const String& op,
    const String& rightPart)
{
  rawEqs_.add(leftPart + op + rightPart);
  const String cstrCoef = "-1.0";
  OSS cstrEquation = OSS();
  if (op.find("<") != std::string::npos)
    cstrEquation << rightPart << cstrCoef << "*("
                 << leftPart << ")";
  else
    cstrEquation << leftPart << cstrCoef << "*("
                 << rightPart << ")";

  if (op == "=")
    eqFunc_.add(cstrEquation.str());
  else
    ineqFunc_.add(cstrEquation.str());
}

void OptimizationAnalysis::resetConstraints()
{
  eqFunc_.clear();
  ineqFunc_.clear();
  rawEqs_.clear();
}

Function OptimizationAnalysis::transformEquations(Description& eqs)
{
  if (!eqs.getSize())
    return Function();
  // Variables collection to construct constraints composed function
  const Description outputs = getPhysicalModel().getOutputNames();
  Description inputs;
  for (UnsignedInteger i = 0; i < getPhysicalModel().getInputNames().getSize(); ++i)
  {
    // Look if variable is defined as both an input and output
    if(!outputs.contains(getPhysicalModel().getInputNames()[i]))
      inputs.add(getPhysicalModel().getInputNames()[i]);
    // If given variable 'X' is an  output, define a dummy 'X0' input variable
    else
    {
      const String varName = getPhysicalModel().getInputNames()[i];
      int j = 0;
      while (getPhysicalModel().hasInputNamed((OSS() << varName << j).str()))
        ++j;
      inputs.add((OSS() << varName << j).str());
    }
  }

  Description vars;
  vars.add(inputs);
  vars.add(outputs);

  // Functions construction
  // Aggregated(X->X, X->Y)
  Collection<Function> funcModelColl;
  const Description modelInputs = getPhysicalModel().getInputNames();
  // X->X
  funcModelColl.add(SymbolicFunction(Tools::GetNormalizedVariables(modelInputs),
                                     Tools::GetNormalizedVariables(modelInputs)));
  // X->Y
  funcModelColl.add(getPhysicalModel().getFunction());
  AggregatedFunction funcModel = AggregatedFunction(funcModelColl);

  // Aggregated C_i
  Collection<Function> funcC_i;
  for(UnsignedInteger i = 0; i < eqs.getSize(); ++i)
    funcC_i.add(SymbolicFunction(Tools::GetNormalizedVariables(vars),
                                 Description(1, eqs[i])));

  return ComposedFunction(AggregatedFunction(funcC_i), funcModel) ;
}

Function OptimizationAnalysis::getEqualityConstraints()
{
  return transformEquations(eqFunc_);
}


Function OptimizationAnalysis::getInequalityConstraints()
{
  return transformEquations(ineqFunc_);
}




Parameters OptimizationAnalysis::getParameters() const
{
  Parameters param;

  param.add("Analysis", "Optimization");
  param.add("Output of interest", getInterestVariables().__str__());
  param.add("Solver", getSolverName());
  param.add("Optimization type", getMinimization() ? "Minimization" : "Maximization");

  String constraints;
  if (getRawEquations().getSize())
    for (UnsignedInteger i = 0; i < getRawEquations().getSize(); ++i)
    {
      constraints += getRawEquations()[i];
      if (i < getRawEquations().getSize() - 1)
        constraints += "\n";
    }
  else
    constraints = "None";
  param.add("Constraints", constraints);

  param.add("Maximum number of calls", getMaximumCallsNumber());
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
  oss << "bounds = ot.Interval(" << Parameters::GetOTPointStr(getBounds().getLowerBound())
      << ", " << Parameters::GetOTPointStr(getBounds().getUpperBound())
      << ", " << Parameters::GetOTBoolCollectionStr(getBounds().getFiniteLowerBound())
      << ", " << Parameters::GetOTBoolCollectionStr(getBounds().getFiniteUpperBound());
  oss << ")\n";

  for (UnsignedInteger i = 0; i < getRawEquations().getSize(); ++i)
    oss << getName() << ".addConstraint(\"" << getRawEquations()[i] << "\")\n";
  oss << getName() << ".setBounds(bounds)\n";
  oss << getName() << ".setStartingPoint(" << Parameters::GetOTPointStr(getStartingPoint()) << ")\n";
  oss << getName() << ".setVariableInputs(" << Parameters::GetOTDescriptionStr(getVariableInputs()) << ")\n";
  oss << getName() << ".setVariablesType(" << Parameters::GetOTIndicesStr(getVariablesType()) << ")\n";

  oss << getName() << ".setMaximumCallsNumber(" << getMaximumCallsNumber() << ")\n";
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
      << " constraints=" << getRawEquations()
      << " maximumCallsNumber=" << getMaximumCallsNumber()
      << " maximumAbsoluteError=" << getMaximumAbsoluteError()
      << " maximumRelativeError=" << getMaximumRelativeError()
      << " maximumResidualError=" << getMaximumResidualError()
      << " maximumConstraintError=" << getMaximumConstraintError()
      << " bounds=" << Parameters::GetOTIntervalDescription(getBounds())
      << " variable inputs=" << getVariableInputs()
      << " variable types=" << getVariablesType();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void OptimizationAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("solverName_", solverName_);
  adv.saveAttribute("isMinimization_", isMinimization_);
  adv.saveAttribute("variablesType_", variablesType_);
  adv.saveAttribute("startingPoint_", startingPoint_);
  adv.saveAttribute("maximumEvaluationNumber_", maximumEvaluationNumber_);
  adv.saveAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.saveAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.saveAttribute("maximumResidualError_", maximumResidualError_);
  adv.saveAttribute("maximumConstraintError_", maximumConstraintError_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("variableInputs_", variableInputs_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("eqFunc_", eqFunc_);
  adv.saveAttribute("ineqFunc_", ineqFunc_);
  adv.saveAttribute("rawEqs_", rawEqs_);

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
  adv.loadAttribute("eqFunc_", eqFunc_);
  adv.loadAttribute("ineqFunc_", ineqFunc_);
  adv.loadAttribute("rawEqs_", rawEqs_);
  if(adv.hasAttribute("variablesType_"))
    adv.loadAttribute("variablesType_", variablesType_);
  else
    variablesType_ = Indices(inputNames_.getSize(), OptimizationProblemImplementation::CONTINUOUS);
}
}
