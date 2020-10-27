//                                               -*- C++ -*-
/**
 *  @brief Fitting test result
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
#ifndef PERSALYS_FITTINGTESTRESULT_HXX
#define PERSALYS_FITTINGTESTRESULT_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Distribution.hxx>
#include <openturns/TestResult.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API FittingTestResult : public OT::PersistentObject
{
  CLASSNAME

public:
  friend class InferenceAnalysis;

  /** Default constructor */
  FittingTestResult();

  /** Virtual constructor */
  FittingTestResult * clone() const override;

  OT::String getVariableName() const;
  OT::Sample getValues() const;
  OT::Collection<OT::Distribution> getTestedDistributions() const;
  OT::Collection<OT::TestResult> getKolmogorovTestResults() const;
  OT::Point getBICResults() const;
  OT::Description getErrorMessages() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::String variableName_;
  OT::Sample values_;
  OT::PersistentCollection< OT::Distribution > testedDistributions_;
  OT::PersistentCollection< OT::TestResult > kolmogorovTestResults_;
  OT::Point bicResults_;
  OT::Description errorMessages_;
};
}
#endif
