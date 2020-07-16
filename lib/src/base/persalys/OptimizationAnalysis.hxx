//                                               -*- C++ -*-
/**
 *  @brief Optimization of a physical model
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
  virtual OptimizationAnalysis * clone() const;

  virtual void setInterestVariables(const OT::Description& variablesNames);

  void updateParameters();

  static OT::Description GetSolverNames();

  OT::String getSolverName() const;
  void setSolverName(const OT::String& name);

  OT::Point getStartingPoint() const;
  void setStartingPoint(const OT::Point & startingPoint);

  /** Maximum evaluations number accessor */
  void setMaximumEvaluationNumber(const OT::UnsignedInteger maximumEvaluationNumber);
  OT::UnsignedInteger getMaximumEvaluationNumber() const;

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

  OT::Interval getBounds() const;
  void setBounds(const OT::Interval & bounds);

  OT::Description getVariableInputs() const;
  void setVariableInputs(const OT::Description& inputs);

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  OT::OptimizationResult getResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters();
  virtual void initialize();
  virtual void launch();
  static OT::Description SolverNames_;

private:
  OT::Description inputNames_;
  OT::String solverName_;
  OT::Bool isMinimization_;
  OT::Point startingPoint_;
  /** Number of outermost iterations (in case of nested iterations) */
  OT::UnsignedInteger maximumEvaluationNumber_;
  OT::Scalar maximumAbsoluteError_;    /**< Value of ||x_n - x_{n-1}|| */
  OT::Scalar maximumRelativeError_;    /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  OT::Scalar maximumResidualError_;    /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  OT::Scalar maximumConstraintError_;  /**< Value of ||constraints(x_n)|| for the active constraints */
  OT::Interval bounds_;
  OT::Description variableInputs_;
  OT::OptimizationResult result_;
};
}
#endif
