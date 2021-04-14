//                                               -*- C++ -*-
/**
 *  @brief Class to define kriging
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
  KrigingAnalysis * clone() const override;

  OT::Basis getBasis() const;
  void setBasis(const OT::Basis& basis);

  OT::CovarianceModel getCovarianceModel() const;
  void setCovarianceModel(const OT::CovarianceModel& model);

  bool getOptimizeParameters() const;
  void setOptimizeParameters(const bool optimize);

  KrigingAnalysisResult getResult() const;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::Function runAlgo(const OT::Sample& inputSample, const OT::Sample& outputSample) override;
  OT::KrigingAlgorithm buildKrigingAlgorithm(const OT::Sample& inputSample,
      const OT::Sample& outputSample,
      const bool useOptimalCovModel = false);

protected:
  void initialize() override;
  void launch() override;
  void validateMetaModelResult(OT::Collection<KrigingAnalysisResult> results, const OT::Sample& inputSample);
  void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample) override;

private:
  OT::Basis basis_;
  OT::CovarianceModel covarianceModel_;
  OT::Function normalization_;
  KrigingAnalysisResult result_;
  OT::CovarianceModel optimalCovarianceModel_;
  bool optimizeParameters_;
};
}
#endif
