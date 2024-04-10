//                                               -*- C++ -*-
/**
 *  @brief Inference analysis result
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
#ifndef PERSALYS_INFERENCERESULT_HXX
#define PERSALYS_INFERENCERESULT_HXX

#include "FittingTestResult.hxx"
#include "AnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API InferenceResult : public AnalysisResult
{
  CLASSNAME

public:
  typedef OT::Collection<FittingTestResult> FittingTestResultCollection;

  friend class InferenceAnalysis;

  /** Default constructor */
  InferenceResult();

  /** Virtual constructor */
  InferenceResult * clone() const override;

  FittingTestResultCollection getFittingTestResultCollection() const;
  FittingTestResult getFittingTestResultForVariable(const OT::String & variableName) const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::PersistentCollection< FittingTestResult > fittingTestResultCollection_;
};
}
#endif
