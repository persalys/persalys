//                                               -*- C++ -*-
/**
 *  @brief MorrisResult contains the Standard Regression Coefficients
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_MORRISRESULT_HXX
#define PERSALYS_MORRISRESULT_HXX

#include "EvaluationResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MorrisResult : public EvaluationResult
{
  CLASSNAME

public:

  friend class MorrisAnalysis;

  /** Default constructor */
  MorrisResult();

  /** Virtual constructor */
  MorrisResult * clone() const override;

  OT::Indices getInputsSelection(const OT::UnsignedInteger marginal) const;
  void setInputsSelection(const OT::UnsignedInteger marginal, const OT::Indices& values);

  OT::Scalar getNoEffectBoundary(const OT::UnsignedInteger marginal) const;
  void setNoEffectBoundary(const OT::UnsignedInteger marginal, const OT::Scalar value);

  // Get Mean/Standard deviation
  OT::Point getMeanAbsoluteElementaryEffects(const OT::UnsignedInteger outputMarginal) const;
  OT::Point getMeanElementaryEffects(const OT::UnsignedInteger outputMarginal) const;
  OT::Point getStandardDeviationElementaryEffects(const OT::UnsignedInteger outputMarginal) const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::PersistentCollection<OT::Indices > inputsSelection_;
  OT::Point noEffectBoundary_;
  // Elementary effects ==> N x (p*q) sample
  OT::Sample elementaryEffectsMean_;
  OT::Sample elementaryEffectsStandardDeviation_;
  OT::Sample absoluteElementaryEffectsMean_;
};
}
#endif
