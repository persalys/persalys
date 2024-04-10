//                                               -*- C++ -*-
/**
 *  @brief Multi-objective optimization of a physical model
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
#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/SymbolicFunction.hxx>
#include <openturns/Pagmo.hxx>
#include <openturns/SimulatedAnnealingLHS.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/UserDefined.hxx>

using namespace OT;

namespace PERSALYS
{

  CLASSNAMEINIT(MultiObjectiveOptimizationAnalysis)
  static Factory<MultiObjectiveOptimizationAnalysis> Factory_MultiObjectiveOptimizationAnalysis;

  CLASSNAMEINIT(MultiObjectiveOptimizationAnalysisResult)
  static Factory<MultiObjectiveOptimizationAnalysisResult> Factory_MultiObjectiveOptimizationAnalysisResult;

  MultiObjectiveOptimizationAnalysisResult::MultiObjectiveOptimizationAnalysisResult()
    : PersistentObject()
  {
  }

  /* Virtual constructor */
  MultiObjectiveOptimizationAnalysisResult* MultiObjectiveOptimizationAnalysisResult::clone() const
  {
    return new MultiObjectiveOptimizationAnalysisResult(*this);
  }

  void MultiObjectiveOptimizationAnalysisResult::save(Advocate & adv) const
  {
    PersistentObject::save(adv);
    adv.saveAttribute("fronts_", fronts_);
    adv.saveAttribute("finalPop_", finalPop_);
  }

  void MultiObjectiveOptimizationAnalysisResult::load(Advocate & adv)
  {
    PersistentObject::load(adv);
    adv.loadAttribute("fronts_", fronts_);
    adv.loadAttribute("finalPop_", finalPop_);
  }

  String MultiObjectiveOptimizationAnalysisResult::__repr__() const
  {
    OSS oss;
    oss << "class=" << GetClassName()
        << " fronts=" << getFronts()
        << " finalPop=" << getFinalPop();
    return oss;
  }

  Description MultiObjectiveOptimizationAnalysis::GetSolverNames(const Interval& bounds,
                                                                 const Indices& types,
                                                                 const Function& eqFunc,
                                                                 const Function& ineqFunc)
  {
    // Dummy function to match a multi objective problem
    Function func = SymbolicFunction(Description::BuildDefault(bounds.getDimension(), "x"),
                                     Description(bounds.getDimension(), "x0^2"));
    OptimizationProblem problem(func, Function(), Function(), bounds);
    if (types.getSize())
      problem.setVariablesType(types);
    if (eqFunc.getInputDimension())
      problem.setEqualityConstraint(func);
    if (ineqFunc.getInputDimension())
      problem.setInequalityConstraint(func);
    return OptimizationAlgorithm::GetAlgorithmNames(problem);
  }

  MultiObjectiveOptimizationAnalysis::MultiObjectiveOptimizationAnalysis()
    : OptimizationAnalysis()
  {
    solverName_ = "nsga2";
  }

  MultiObjectiveOptimizationAnalysis::MultiObjectiveOptimizationAnalysis(const OT::String & name,
                                                                         const PhysicalModel & physicalModel,
                                                                         const OT::String & solverName)
    : OptimizationAnalysis(name, physicalModel, solverName)
  {
    initializeParameters();
    if (getPhysicalModel().getSelectedOutputsNames().getSize() > 1) {
      setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
      setMinimization(getPhysicalModel().getSelectedOutputsNames());
    }
  }

  /* Virtual constructor */
  MultiObjectiveOptimizationAnalysis* MultiObjectiveOptimizationAnalysis::clone() const
  {
    return new MultiObjectiveOptimizationAnalysis(*this);
  }

  void MultiObjectiveOptimizationAnalysis::setInterestVariables(const Description& variablesNames)
  {
    if (variablesNames.getSize() < 2)
      throw InvalidArgumentException(HERE) << "Define at least two variables of interest";
    // If new interest variable: add it as a minimization objective
    for (UnsignedInteger i=0; i<variablesNames.getSize(); ++i)
      if (!getInterestVariables().contains(variablesNames[i]))
        areMinimization_.add(variablesNames[i]);
    AnalysisImplementation::setInterestVariables(variablesNames);
  }

  bool MultiObjectiveOptimizationAnalysis::canBeLaunched(String &errorMessage) const
  {
    const bool canBeLaunched = PhysicalModelAnalysis::canBeLaunched(errorMessage);
    if (!canBeLaunched)
      return false;
    if (getPhysicalModel().getOutputDimension() < 2)
      errorMessage = "The physical model must have at least two outputs.";
    return errorMessage.empty();
  }

  void MultiObjectiveOptimizationAnalysis::updateParameters()
  {
    const Description inputNames(inputNames_);
    const Point startingPoint(startingPoint_);
    const Point lowerBounds(bounds_.getLowerBound());
    const Point upperBounds(bounds_.getUpperBound());
    const Description variableInputs(variableInputs_);

    initializeParameters();

    Point newLowerBounds(bounds_.getLowerBound());
    Point newUpperBounds(bounds_.getUpperBound());

    for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
    {
      const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
      if (it != inputNames.end())
      {
        startingPoint_[i] = startingPoint[it - inputNames.begin()];
        newLowerBounds[i] = lowerBounds[it - inputNames.begin()];
        newUpperBounds[i] = upperBounds[it - inputNames.begin()];
        if (!variableInputs.contains(inputNames_[i]))
          variableInputs_.erase(std::remove(variableInputs_.begin(), variableInputs_.end(), inputNames_[i]), variableInputs_.end());
      }
    }
    bounds_.setLowerBound(newLowerBounds);
    bounds_.setUpperBound(newUpperBounds);
  }

  void MultiObjectiveOptimizationAnalysis::initialize()
  {
    PhysicalModelAnalysis::initialize();
    notify("progressValueChanged");
  }

  void MultiObjectiveOptimizationAnalysis::launch()
  {
    RandomGenerator::SetSeed(getSeed());
    const Description modelInputNames(getPhysicalModel().getInputNames());
    const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();
    // check
    if (getInterestVariables().getSize() < 2)
      throw InvalidArgumentException(HERE) << "Specify at least two variables to optimize";
    if (startingPoint_.getDimension() != nbInputs)
      throw InvalidArgumentException(HERE) << "Invalid starting point";
    if (variableInputs_.getSize() < 2)
      throw InvalidArgumentException(HERE) << "At least two variables must vary";

    // set OptimizationProblem
    OptimizationProblem problem = defineProblem();

    for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
      if (getMinimization().contains(getInterestVariables()[i]))
        problem.setMinimization(true, i);
      else
        problem.setMinimization(false, i);

    // Generate initial population
    Collection<Distribution> distColl;

    for (UnsignedInteger i=0; i<getVariableInputs().getSize(); ++i) {
      const Scalar lb = bounds_.getLowerBound()[variableInputsIndices_[i]];
      const Scalar ub = bounds_.getUpperBound()[variableInputsIndices_[i]];
      if (!(lb < ub))
        throw InvalidArgumentException(HERE) << "Interval is empty for variable " << getVariableInputs()[i];
      if (getVariablesType()[variableInputsIndices_[i]] == OptimizationProblemImplementation::CONTINUOUS)
        distColl.add(OT::Uniform(lb, ub));
      else if (getVariablesType()[variableInputsIndices_[i]] == OptimizationProblemImplementation::BINARY) {
        Sample values(2, 1);
        values(1, 0) = 1.;
        distColl.add(OT::UserDefined(values, Point(2, 0.5)));
      } else {
        Sample values;
        for (int val=lb; val<=ub; ++val) {
          const Point point(1, val);
          values.add(point);
        }
        distColl.add(OT::UserDefined(values));
      }
    }

    ComposedDistribution distribution(distColl);
    SimulatedAnnealingLHS doe(LHSExperiment(distribution, startingPopSize_),
                              SpaceFillingPhiP(), GeometricProfile());

    const Sample startingPop = doe.generate();

    // build solver
    Pagmo solver(problem, solverName_, startingPop);
#if OPENTURNS_VERSION >= 102300
    solver.setMaximumIterationNumber(generationNumber_);
#else
    solver.setGenerationNumber(generationNumber_);
#endif
    solver.setStopCallback(&AnalysisImplementation::Stop, this);
    solver.setProgressCallback(&UpdateProgressValue, this);
    solver.setMaximumConstraintError(getMaximumConstraintError());

    // run solver
    solver.run();

    // set result
    moResult_ = MultiObjectiveOptimizationAnalysisResult();

    // initialize final pop sample
    // X | Y | C_i | _feasibility_ | _front_index_

    Sample finalPop = solver.getResult().getFinalPoints();
    finalPop.stack(solver.getResult().getFinalValues());
    Description finalPopDesc = getVariableInputs();
    finalPopDesc.add(getInterestVariables());

    // If constraints, add them and feasibility
    const Bool isConstrained = problem.getEqualityConstraint().getInputDimension() ||
      problem.getInequalityConstraint().getInputDimension();

    if (isConstrained)
    {
      // Build constraints sample
      for (UnsignedInteger i=0; i<getRawEquations().getSize(); ++i)
      {
        std::regex variable("([_a-zA-Z][_a-zA-Z0-9]*)");
        std::smatch match;
        std::regex_search(getRawEquations()[i], match, variable);
        // If constraint variable is not already a model output
        if (!finalPopDesc.contains(match[1]))
        {
          finalPopDesc.add(Description(1, match[1]));
          finalPop.stack(getPhysicalModel().getFunction(Description(1, match[1]))(solver.getResult().getFinalPoints()));
        }
      }
      finalPop.stack(Sample(finalPop.getSize(), Point(1, 0)));
      finalPopDesc.add(Description(1, "_feasibility_"));
    }

    // add fronts last
    finalPop.stack(Sample(finalPop.getSize(), Point(1, -1)));
    finalPopDesc.add(Description(1, "_front_index_"));
    finalPop.setDescription(finalPopDesc);

    // build fronts sample collection
    Collection<Sample> fronts;
    const IndicesCollection frontIndices = solver.getResult().getParetoFrontsIndices();
    for (UnsignedInteger i=0; i<frontIndices.getSize(); ++i) {
      // select points belonging to front
      const Indices fronti(frontIndices.cbegin_at(i), frontIndices.cend_at(i));
      fronts.add(solver.getResult().getFinalValues().select(fronti));
      fronts[i].setDescription(getInterestVariables());
      // add front index and feasibility to final pop sample last 2 columns
      for (UnsignedInteger j=0; j<fronti.getSize(); ++j) {
        finalPop(fronti[j], finalPop.getDimension() - 1) = i;
        if (isConstrained)
        {
          const Point point = solver.getResult().getFinalPoints()[fronti[j]];
          bool isFeasible = true;
          if (problem.getEqualityConstraint().getInputDimension()) {
            for (UnsignedInteger k=0; k<problem.getEqualityConstraint().getOutputDimension(); ++k) {
              isFeasible &= std::abs(problem.getEqualityConstraint()(point)[k]) < getMaximumConstraintError();
            }
          }
          if (problem.getInequalityConstraint().getInputDimension()) {
            for (UnsignedInteger k=0; k<problem.getInequalityConstraint().getOutputDimension(); ++k) {
              isFeasible &= problem.getInequalityConstraint()(point)[k] > 0.0;
            }
          }
          finalPop(fronti[j], finalPop.getDimension() - 2) = (int)isFeasible;
        }
      }
    }
    moResult_.setFinalPop(finalPop);
    moResult_.setFronts(fronts);
  }


  MultiObjectiveOptimizationAnalysisResult MultiObjectiveOptimizationAnalysis::getResult() const
  {
    return moResult_;
  }

  bool MultiObjectiveOptimizationAnalysis::hasValidResult() const
  {
    return moResult_.getFronts().getSize();
  }


  void MultiObjectiveOptimizationAnalysis::setSolverName(const String& name)
  {
    if (!GetSolverNames(getBounds()).contains(name))
      throw InvalidArgumentException(HERE) << "Error: the given solver name=" << name << " is unknown. " << GetSolverNames(getBounds());
    solverName_ = name;
  }

  void MultiObjectiveOptimizationAnalysis::setPopulationSize(const UnsignedInteger startingPopSize)
  {
    if (!startingPopSize)
      throw InvalidArgumentException(HERE) << "The starting population size must be larger than 0";
    startingPopSize_ = startingPopSize;
  }


  UnsignedInteger MultiObjectiveOptimizationAnalysis::getPopulationSize() const
  {
    return startingPopSize_;
  }

  void MultiObjectiveOptimizationAnalysis::setMinimization(const Description& varNames)
  {
    areMinimization_ = varNames;
  }


  Description MultiObjectiveOptimizationAnalysis::getMinimization() const
  {
    return areMinimization_;
  }


  void MultiObjectiveOptimizationAnalysis::setGenerationNumber(const UnsignedInteger generationNumber)
  {
    generationNumber_ = generationNumber;
  }


  UnsignedInteger MultiObjectiveOptimizationAnalysis::getGenerationNumber() const
  {
    return generationNumber_;
  }


  String MultiObjectiveOptimizationAnalysis::getPythonScript() const
  {
    OSS oss;

    oss << getName() << " = persalys.MultiObjectiveOptimizationAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ", '" << solverName_ << "')\n";
    // interest output
    if (getInterestVariables().getSize() < getPhysicalModel().getOutputNames().getSize())
    {
      oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
      oss << getName() << ".setInterestVariables(interestVariables)\n";
    }
    oss << getName() << ".setMinimization(" << Parameters::GetOTDescriptionStr(getMinimization()) << ")\n";
    oss << "bounds = ot.Interval(" << Parameters::GetOTPointStr(getBounds().getLowerBound())
        << ", " << Parameters::GetOTPointStr(getBounds().getUpperBound());
    oss << ")\n";

    for (UnsignedInteger i = 0; i < getRawEquations().getSize(); ++i)
      oss << getName() << ".addConstraint(\"" << getRawEquations()[i] << "\")\n";
    oss << getName() << ".setBounds(bounds)\n";
    oss << getName() << ".setPopulationSize(" << getPopulationSize() << ")\n";
    oss << getName() << ".setGenerationNumber(" << getGenerationNumber() << ")\n";

    oss << getName() << ".setVariableInputs(" << Parameters::GetOTDescriptionStr(getVariableInputs()) << ")\n";
    oss << getName() << ".setVariablesType(" << Parameters::GetOTIndicesStr(getVariablesType()) << ")\n";
    return oss;
}

  /* String converter */
  String MultiObjectiveOptimizationAnalysis::__repr__() const
  {
    OSS oss;
    oss << "class=" << GetClassName()
        << " name=" << getName()
        << " physicalModel=" << getPhysicalModel().getName()
        << " algorithmName=" << getSolverName()
        << " number of generations=" << getGenerationNumber()
        << " starting population size=" << getPopulationSize()
        << " bounds=" << Parameters::GetOTIntervalDescription(getBounds())
        << " variable inputs=" << getVariableInputs()
        << " variable types=" << getVariablesType()
        << " variables of interest=" << getInterestVariables()
        << " minimization=" << getMinimization()
        << " constraints=" << getRawEquations()
        << " maximumConstraintError=" << getMaximumConstraintError()
        << " seed=" << getSeed();
    return oss;
  }

  Parameters MultiObjectiveOptimizationAnalysis::getParameters() const
  {
    Parameters param;

    param.add("Analysis", "Multi-objective optimization");
    param.add("Outputs of interest", getInterestVariables().__str__());
    param.add("Solver", getSolverName());

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
    param.add("Bounds", varInputs);
    param.add("Number of generations", getGenerationNumber());
    param.add("Initial population size", getPopulationSize());

    if (fixedInputs.getSize())
      param.add("Fixed inputs", fixedInputs.__str__());

    String objectives;
    for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
    {
      if (getMinimization().contains(getInterestVariables()[i]))
        objectives += getInterestVariables()[i] + " : Minimization";
      else
        objectives += getInterestVariables()[i] + " : Maximization";
      if (i < getInterestVariables().getSize() - 1)
        objectives += "\n";
    }

    param.add("Objectives", objectives);

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
    param.add("Seed", getSeed());

    return param;
  }

  void MultiObjectiveOptimizationAnalysis::save(Advocate & adv) const
  {
    OptimizationAnalysis::save(adv);
    adv.saveAttribute("startingPopSize_", startingPopSize_);
    adv.saveAttribute("generationNumber_" , generationNumber_);
    adv.saveAttribute("seed_", seed_);
    adv.saveAttribute("areMinimization_", areMinimization_);
    adv.saveAttribute("moResult_", moResult_);
  }

  void MultiObjectiveOptimizationAnalysis::load(Advocate & adv)
  {
    OptimizationAnalysis::load(adv);
    adv.loadAttribute("startingPopSize_", startingPopSize_);
    adv.loadAttribute("generationNumber_" , generationNumber_);
    adv.loadAttribute("seed_", seed_);
    adv.loadAttribute("areMinimization_", areMinimization_);
    adv.loadAttribute("moResult_", moResult_);
  }


}
