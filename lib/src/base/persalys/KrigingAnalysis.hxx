//                                               -*- C++ -*-
/**
 *  @brief Class to define kriging
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
#ifndef PERSALYS_KRIGINGANALYSIS_HXX
#define PERSALYS_KRIGINGANALYSIS_HXX

#include "MetaModelAnalysis.hxx"
#include "Analysis.hxx"
#include "KrigingAnalysisResult.hxx"

#include <openturns/KrigingAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API KrigingAnalysis : public MetaModelAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  KrigingAnalysis();
  /** Constructor with parameters */
  KrigingAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);
  KrigingAnalysis(const OT::String& name, const Analysis& designOfExperimentAnalysis);

  /** Virtual constructor */
  virtual KrigingAnalysis * clone() const;

  OT::Basis getBasis() const;
  void setBasis(const OT::Basis& basis);

  OT::CovarianceModel getCovarianceModel() const;
  void setCovarianceModel(const OT::CovarianceModel& model);

  bool getOptimizeParameters() const;
  void setOptimizeParameters(const bool optimize);

  KrigingAnalysisResult getResult() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  virtual OT::Function runAlgo(const OT::Sample& inputSample, const OT::Sample& outputSample);
  OT::KrigingAlgorithm buildKrigingAlgorithm(const OT::Sample& inputSample,
      const OT::Sample& outputSample,
      const bool useOptimalCovModel = false);

protected:
  virtual void initialize();
  virtual void launch();
  void validateMetaModelResult(OT::Collection<KrigingAnalysisResult> results, const OT::Sample& inputSample);
  virtual void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);

private:
  OT::Basis basis_;
  OT::CovarianceModel covarianceModel_;
  KrigingAnalysisResult result_;
  OT::CovarianceModel optimalCovarianceModel_;
  bool optimizeParameters_;
};
}
#endif
