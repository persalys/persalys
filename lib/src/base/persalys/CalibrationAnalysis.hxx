//                                               -*- C++ -*-
/**
 *  @brief Calibration analysis
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_CALIBRATIONANALYSIS_HXX
#define PERSALYS_CALIBRATIONANALYSIS_HXX

#include "PhysicalModelAnalysis.hxx"
#include "DesignOfExperiment.hxx"
#include "CalibrationAnalysisResult.hxx"

#include <openturns/CalibrationResult.hxx>
#include <openturns/OptimizationAlgorithm.hxx>

namespace PERSALYS
{

class PERSALYS_API CalibrationAnalysis : public PhysicalModelAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  CalibrationAnalysis();
  /** Constructor with parameters */
  CalibrationAnalysis(const OT::String& name, const DesignOfExperiment& observations);
  /** Virtual constructor */
  virtual CalibrationAnalysis * clone() const;

  DesignOfExperiment getObservations() const {return observations_;};

  CalibrationAnalysisResult getResult() const {return result_;};

  void updateParameters();

  static OT::Description GetMethodNames();

  OT::String getMethodName() const {return methodName_;};
  void setMethodName(const OT::String &name);

  OT::OptimizationAlgorithm getOptimizationAlgorithm() const {return optimizationAlgorithm_;};
  void setOptimizationAlgorithm(const OT::OptimizationAlgorithm& solver);

  OT::PointWithDescription getFixedInputs() const {return fixedValues_;};

  OT::Description getCalibratedInputs() const {return priorDistribution_.getDescription();};
  void setCalibratedInputs(const OT::Description &calibratedInputs, const OT::Distribution &priorDistribution,
                           const OT::Description &fixedInputs=OT::Description(), const OT::Point &fixedValues=OT::Point());

  OT::Distribution getPriorDistribution() const {return priorDistribution_;};

  OT::CovarianceMatrix getErrorCovariance() const {return errorCovariance_;};
  void setErrorCovariance(const OT::CovarianceMatrix& matrix);

  OT::Scalar getConfidenceIntervalLength() const {return confidenceIntervalLength_;};
  void setConfidenceIntervalLength(const OT::Scalar);

  OT::UnsignedInteger getBootStrapSize() const {return bootStrapSize_;};
  void setBootStrapSize(const OT::UnsignedInteger);

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters();
  virtual void initialize();
  void check(const OT::Description &calibratedInputs, const OT::Distribution &priorDistribution,
             const OT::Description &fixedInputs, const OT::Point &fixedValues);
  virtual void launch();

  template <class T>
  void runNonLinearAlgorithm(T& algo);

  void runCalibrationAlgorithm(const OT::Function& paramFunction);

private:
  static OT::Description MethodNames_;
  OT::String methodName_;
  OT::Scalar confidenceIntervalLength_;
  OT::UnsignedInteger bootStrapSize_;
  OT::OptimizationAlgorithm optimizationAlgorithm_;
  DesignOfExperiment observations_;
  OT::Distribution priorDistribution_;
  OT::PointWithDescription fixedValues_;
  OT::CovarianceMatrix errorCovariance_;
  CalibrationAnalysisResult result_;
};
}
#endif
