//                                               -*- C++ -*-
/**
 *  @brief Linear metamodel
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_POLYNOMIALREGRESSIONANALYSIS_HXX
#define PERSALYS_POLYNOMIALREGRESSIONANALYSIS_HXX

#include "MetaModelAnalysis.hxx"
#include "Analysis.hxx"
#include "PolynomialRegressionAnalysisResult.hxx"

#include <openturns/LinearModelStepwiseAlgorithm.hxx>
#include <openturns/LinearModelAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API PolynomialRegressionAnalysis : public MetaModelAnalysis
{
  CLASSNAME

private:
  class Algorithm
  {
    public:
      Algorithm() {}

      Algorithm(const OT::LinearModelAlgorithm &algo)
      : stepwise_(false)
      , algo_(algo)
      {}

      Algorithm(const OT::LinearModelStepwiseAlgorithm &algo)
      : stepwise_(true)
      , stepwiseAlgo_(algo)
      {}

      inline void run() {
        stepwise_ ? stepwiseAlgo_.run() : algo_.run();
      }

      inline OT::LinearModelResult getResult() {
        return stepwise_ ? stepwiseAlgo_.getResult() : algo_.getResult();
      }

      void setPenalty(const OT::Scalar penalty) {
        if (!stepwise_)
          throw OT::NotDefinedException(HERE) << "Cannot set penalty to non stepwise regression";
        stepwiseAlgo_.setPenalty(penalty);
      }

    private:
      bool stepwise_;
      OT::LinearModelAlgorithm algo_;
      OT::LinearModelStepwiseAlgorithm stepwiseAlgo_;
  };

public:
  enum PenaltyCriteria {AIC, BIC};

  /** Default constructor */
  PolynomialRegressionAnalysis();

  /** Constructor with parameters */
  PolynomialRegressionAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);
  PolynomialRegressionAnalysis(const OT::String& name, const Analysis& designOfExperimentAnalysis);

  /** Virtual constructor */
  PolynomialRegressionAnalysis * clone() const override;

  OT::UnsignedInteger getDegree() const;
  void setDegree(const OT::UnsignedInteger degree);

  OT::Bool getInteraction() const;
  void setInteraction(const OT::Bool interaction);

  OT::Bool getStepwise() const;
  void setStepwise(const OT::Bool stepwise);

  OT::LinearModelStepwiseAlgorithm::Direction getDirection() const;
  void setDirection(const OT::LinearModelStepwiseAlgorithm::Direction direction);

  PenaltyCriteria getPenalty() const;
  void setPenalty(const PenaltyCriteria penalty);

  PolynomialRegressionAnalysisResult getResult() const;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  PhysicalModel getMetaModel() const override;

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
  OT::Collection<OT::LinearModelResult> computeResults();
  void treatResults(const OT::Collection<OT::LinearModelResult> & results);
  OT::Function runAlgoMarginal(const OT::Sample& inputSample, const OT::Sample& outputSample) override;
  Algorithm buildAlgo(const OT::Sample & inputSample, const OT::Sample & outputSample) const;
  OT::Basis getBasis() const;

private:
  PolynomialRegressionAnalysisResult          result_;
  OT::Bool                                    interaction_  = false;
  OT::UnsignedInteger                         degree_       = 1u;
  OT::Bool                                    stepwise_     = true;
  OT::LinearModelStepwiseAlgorithm::Direction direction_    = OT::LinearModelStepwiseAlgorithm::BOTH;
  PenaltyCriteria                             penalty_      = PenaltyCriteria::BIC;
};
}
#endif
