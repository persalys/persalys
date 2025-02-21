//                                               -*- C++ -*-
/**
 *  @brief Optimization of a physical model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_OPTIMIZATIONANALYSIS_HXX
#define PERSALYS_OPTIMIZATIONANALYSIS_HXX

#include "PhysicalModelAnalysis.hxx"

#include <openturns/OptimizationResult.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API AlgorithmProperty
{
public:
  enum Locality {Local, Global, AnyL};
  enum Derivative {None, First, AnyD};
  enum Priority {Low, Medium, High};
  AlgorithmProperty() {};
  AlgorithmProperty(Locality locality, Derivative derivative, OT::String doc, Priority priority = Low)
    : locality_(locality)
    , derivative_(derivative)
    , doc_(doc)
    , priority_(priority) {};
  Locality getLocality() const
  {
    return locality_;
  };
  Derivative getDerivative() const
  {
    return derivative_;
  }
  OT::String getDoc() const
  {
    return doc_;
  }
  Priority getPriority() const
  {
    return priority_;
  };

private:
  Locality locality_;
  Derivative derivative_;
  OT::String doc_;
  Priority priority_;

};


class PERSALYS_BASE_API OptimizationAnalysis : public PhysicalModelAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  OptimizationAnalysis();

  /** Constructor with parameters */
  OptimizationAnalysis(const OT::String & name,
                       const PhysicalModel & physicalModel,
                       const OT::String & solverName = "Cobyla");

  /** Virtual constructor */
  OptimizationAnalysis * clone() const override;

  void setInterestVariables(const OT::Description& variablesNames) override;

  virtual void updateParameters();

  static OT::Description GetSolverNames();
  static OT::Description GetSolverNames(const OT::Interval& bounds,
                                        const OT::Indices& types = OT::Indices(),
                                        const OT::Function& eqFunc = OT::Function(),
                                        const OT::Function& ineqFunc = OT::Function());

  static std::map<OT::String, AlgorithmProperty> AlgorithmDictionary;

  OT::String getSolverName() const;
  virtual void setSolverName(const OT::String& name);

  OT::Point getStartingPoint() const;
  void setStartingPoint(const OT::Point & startingPoint);

  OT::OptimizationProblem defineProblem();

  /** Constraints **/
  void addConstraint(const OT::String& leftPart, const OT::String& op,
                     const OT::String& rightPart);
  void addConstraint(const OT::String& equation);
  void resetConstraints();
  OT::Description getRawEquations() const
  {
    return rawEqs_;
  };

  /** Maximum evaluations number accessor */
  void setMaximumCallsNumber(const OT::UnsignedInteger maximumCallsNumber);
  OT::UnsignedInteger getMaximumCallsNumber() const;

  /** Maximum absolute error accessor */
  OT::Scalar getMaximumAbsoluteError() const;
  void setMaximumAbsoluteError(const OT::Scalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  OT::Scalar getMaximumRelativeError() const;
  void setMaximumRelativeError(const OT::Scalar maximumRelativeError);

  /** Maximum residual error accessor */
  OT::Scalar getMaximumResidualError() const;
  void setMaximumResidualError(const OT::Scalar maximumResidualError);

  /** Maximum constraint error accessor */
  OT::Scalar getMaximumConstraintError() const;
  void setMaximumConstraintError(const OT::Scalar maximumConstraintError);

  OT::Bool getMinimization() const;
  void setMinimization(const OT::Bool minimization);

  OT::Indices getVariablesType() const;
  void setVariablesType(const OT::Indices& variablesType);

  OT::Interval getBounds() const;
  void setBounds(const OT::Interval & bounds);

  OT::Description getVariableInputs() const;
  void setVariableInputs(const OT::Description& inputs);

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;
  OT::Function getEqualityConstraints();
  OT::Function getInequalityConstraints();

  OT::OptimizationResult getResult() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initializeParameters();
  void initialize() override;
  void launch() override;
  OT::Function transformEquations(OT::Description& eqs);
  OT::Description inputNames_;
  OT::String solverName_;
  OT::Point startingPoint_;
  OT::Interval bounds_;
  OT::Description variableInputs_;
  OT::OptimizationResult result_;
  OT::Description eqFunc_;
  OT::Description ineqFunc_;
  OT::Description rawEqs_;
  OT::Indices variableInputsIndices_;
  OT::Indices variablesType_;


private:
  OT::Bool isMinimization_ = true;
  /** Number of outermost iterations (in case of nested iterations) */
  OT::UnsignedInteger maximumEvaluationNumber_;
  OT::Scalar maximumAbsoluteError_;    /**< Value of ||x_n - x_{n-1}|| */
  OT::Scalar maximumRelativeError_;    /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  OT::Scalar maximumResidualError_;    /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  OT::Scalar maximumConstraintError_;  /**< Value of ||constraints(x_n)|| for the active constraints */
  OT::Point variableInputsValues_;


};
}
#endif
