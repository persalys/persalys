//                                               -*- C++ -*-
/**
 *  @brief Linear metamodel
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_LINEARREGRESSIONANALYSIS_HXX
#define PERSALYS_LINEARREGRESSIONANALYSIS_HXX

#include "MetaModelAnalysis.hxx"
#include "Analysis.hxx"
#include "LinearRegressionAnalysisResult.hxx"

#include <openturns/LinearModelStepwiseAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API LinearRegressionAnalysis : public MetaModelAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  LinearRegressionAnalysis();

  /** Constructor with parameters */
  LinearRegressionAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);
  LinearRegressionAnalysis(const OT::String& name, const Analysis& designOfExperimentAnalysis);

  /** Virtual constructor */
  LinearRegressionAnalysis * clone() const override;

  OT::UnsignedInteger getDegree() const;
  void setDegree(const OT::UnsignedInteger degree);

  OT::Bool getInteraction() const;
  void setInteraction(const OT::Bool interaction);

  LinearRegressionAnalysisResult getResult() const;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initialize() override;
  void launch() override;
  void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample) override;

private:
  OT::Function runAlgoMarginal(const OT::Sample& inputSample, const OT::Sample& outputSample) override;
  OT::LinearModelStepwiseAlgorithm buildAlgo(const OT::Sample & inputSample, const OT::Sample & outputSample);
  OT::Basis getBasis() const;

  LinearRegressionAnalysisResult result_;
  OT::Bool interaction_ = false;
  OT::UnsignedInteger degree_ = 1;
};
}
#endif
