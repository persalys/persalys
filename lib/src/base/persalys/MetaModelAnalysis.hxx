//                                               -*- C++ -*-
/**
 *  @brief Base class to define metamodel
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "PythonPhysicalModel.hxx"

namespace PERSALYS
{

class Study;

class PERSALYS_BASE_API MetaModelAnalysis : public DesignOfExperimentAnalysis
{
public:
  /** Default constructor */
  MetaModelAnalysis();
  /** Constructor with parameters */
  MetaModelAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);

  PythonPhysicalModel asPythonPhysicalModel(const Study &study) const; 

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

  virtual PhysicalModel getMetaModel() const;

  bool canBeLaunched(OT::String &errorMessage) const override;

  static bool CanBeLaunched(OT::String &errorMessage, const DesignOfExperiment &doe);

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv) override;

protected:
  // n-d algos (chaos) should reimplement runAlgo, 1-d algos (lm) to reimplement runAlgoMarginal
  virtual OT::Function runAlgo(const OT::Sample& inputSample, const OT::Sample& outputSample);
  virtual OT::Function runAlgoMarginal(const OT::Sample& inputSample, const OT::Sample& outputSample);

  void buildMetaModel(MetaModelAnalysisResult& result, const OT::Function& function);
  void computeError(const OT::Sample& metaOutSample, const OT::Sample& outSample, OT::Point& mse, OT::Point& q2);
  void validateMetaModelResult(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  virtual void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  void computeTestSampleValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  void computeKFoldValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  void computeLOOValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);
  OT::PointWithDescription getTestSampleValidationParameters() const;
  OT::PointWithDescription getKFoldValidationParameters() const;

protected:
  OT::Distribution distribution_;
  bool isDistributionComputed_ = false;
  bool analyticalValidation_ = true;
  bool testSampleValidation_ = false;
  bool kFoldValidation_ = false;
  bool leaveOneOutValidation_ = false;
  OT::UnsignedInteger percentageTestSample_ = 20;
  OT::UnsignedInteger seedTestSample_ = 0;
  OT::UnsignedInteger nbFolds_ = 5;
  OT::UnsignedInteger seedKFold_ = 0;
};
}
#endif
