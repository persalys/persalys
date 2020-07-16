//                                               -*- C++ -*-
/**
 *  @brief Base class to define metamodel
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
#ifndef PERSALYS_METAMODELANALYSIS_HXX
#define PERSALYS_METAMODELANALYSIS_HXX

#include "DesignOfExperimentAnalysis.hxx"
#include "Analysis.hxx"
#include "MetaModelAnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MetaModelAnalysis : public DesignOfExperimentAnalysis
{
public:
  /** Default constructor */
  MetaModelAnalysis();
  /** Constructor with parameters */
  MetaModelAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);
  MetaModelAnalysis(const OT::String& name, const Analysis& analysis);

  bool analyticalValidation() const;
  void setAnalyticalValidation(const bool validation);

  bool testSampleValidation() const;
  void setTestSampleValidation(const bool validation);

  bool kFoldValidation() const;
  void setKFoldValidation(const bool validation);

  bool leaveOneOutValidation() const;
  void setLeaveOneOutValidation(const bool validation);

  void setTestSampleValidationSeed(const OT::UnsignedInteger seed);
  void setTestSampleValidationPercentageOfPoints(const OT::UnsignedInteger percentage);
  OT::UnsignedInteger getTestSampleValidationPercentageOfPoints() const;
  OT::UnsignedInteger getTestSampleValidationSeed() const;

  void setKFoldValidationNumberOfFolds(const OT::UnsignedInteger nbFolds);
  void setKFoldValidationSeed(const OT::UnsignedInteger seed);
  OT::UnsignedInteger getKFoldValidationNumberOfFolds() const;
  OT::UnsignedInteger getKFoldValidationSeed() const;

  OT::Sample getEffectiveInputSample() const;
  OT::Sample getEffectiveOutputSample() const;

  virtual bool canBeLaunched(OT::String &errorMessage) const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv);

protected:
  virtual OT::Function runAlgo(const OT::Sample& inputSample, const OT::Sample& outputSample) = 0;
  void buildMetaModel(MetaModelAnalysisResult& result, const OT::Function& function);
  void computeError(const OT::Sample& metaOutSample, const OT::Sample& outSample, OT::Point& error, OT::Point& q2);
  void validateMetaModelResult(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  virtual void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  void computeTestSampleValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  void computeKFoldValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  void computeLOOValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  OT::PointWithDescription getTestSampleValidationParameters() const;
  OT::PointWithDescription getKFoldValidationParameters() const;

protected:
  OT::Distribution distribution_;
  bool isDistributionComputed_;
  bool analyticalValidation_;
  bool testSampleValidation_;
  bool kFoldValidation_;
  bool leaveOneOutValidation_;
  OT::UnsignedInteger percentageTestSample_;
  OT::UnsignedInteger seedTestSample_;
  OT::UnsignedInteger nbFolds_;
  OT::UnsignedInteger seedKFold_;
};
}
#endif
