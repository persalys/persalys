//                                               -*- C++ -*-
/**
 *  @brief Results of a simulation analysis
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
#ifndef PERSALYS_EVALUATIONRESULT_HXX
#define PERSALYS_EVALUATIONRESULT_HXX

#include "AnalysisResult.hxx"
#include "DesignOfExperiment.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API EvaluationResult : public AnalysisResult
{
  CLASSNAME

public:
  friend class DesignOfExperimentEvaluation;
  friend class ModelEvaluation;
  friend class FieldModelEvaluation;

  /** Default constructor */
  EvaluationResult();

  /** Constructor with parameters */
  EvaluationResult(const DesignOfExperiment& design);

  /** Virtual constructor */
  EvaluationResult * clone() const override;

  DesignOfExperiment getDesignOfExperiment() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  DesignOfExperiment designOfExperiment_;
};
}
#endif
