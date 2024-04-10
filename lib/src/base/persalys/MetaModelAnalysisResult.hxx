//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel result
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
#ifndef PERSALYS_METAMODELANALYSISRESULT_HXX
#define PERSALYS_METAMODELANALYSISRESULT_HXX

#include "PhysicalModel.hxx"

#include "AnalysisResult.hxx"
#include "MetaModelValidationResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MetaModelAnalysisResult : public AnalysisResult
{
  CLASSNAME

public:

  friend class FunctionalChaosAnalysis;
  friend class KrigingAnalysis;
  friend class PolynomialRegressionAnalysis;
  friend class MetaModelAnalysis;

  /** Default constructor */
  MetaModelAnalysisResult();

  /** Virtual constructor */
  MetaModelAnalysisResult * clone() const override;

  PhysicalModel getMetaModel() const;

  OT::Sample getOutputSample() const;

  OT::Sample getMetaModelOutputSample() const;

  MetaModelValidationResult getAnalyticalValidation() const;
  MetaModelValidationResult getTestSampleValidation() const;
  MetaModelValidationResult getKFoldValidation() const;
  MetaModelValidationResult getLeaveOneOutValidation() const;

  std::vector<MetaModelValidationResult> getValidations() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  PhysicalModel metaModel_;
  MetaModelValidationResult analyticalValidation_;
  MetaModelValidationResult testSampleValidation_;
  MetaModelValidationResult kFoldValidation_;
  MetaModelValidationResult looValidation_;
  OT::Sample outputSample_;
  OT::Sample metaModelOutputSample_;
};
}
#endif
