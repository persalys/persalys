//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
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
#ifndef PERSALYS_FIELDMONTECARLOANALYSIS_HXX
#define PERSALYS_FIELDMONTECARLOANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "FieldMonteCarloResult.hxx"

namespace PERSALYS
{

class PERSALYS_BASE_API CovFunctionEvaluation : public OT::EvaluationImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  CovFunctionEvaluation()
  : OT::EvaluationImplementation()
  , covModel_()
  , inputDimension_(2)
  , outputDimension_(1)
  {};
  /** Default constructor */
  CovFunctionEvaluation(const OT::CovarianceModel& covModel)
  : OT::EvaluationImplementation()
  , covModel_(covModel)
  , inputDimension_(2)
  , outputDimension_(1)
  {};

  /** Virtual constructor */
  CovFunctionEvaluation * clone() const override
  {
    return new CovFunctionEvaluation(*this);
  };

  /** Accessor for input point dimension */
  OT::UnsignedInteger getInputDimension() const override
  {
    return inputDimension_;
  };
  OT::UnsignedInteger getOutputDimension() const override
  {
    return outputDimension_;
  };
  OT::Point operator() (const OT::Point & inP) const override
  {
    OT::Scalar den = std::sqrt(covModel_(inP[0], inP[0])(0, 0) * covModel_(inP[1],  inP[1])(0, 0));
    if (den == 0.0)
        return OT::Point(1, 0.);
    return OT::Point(1, covModel_(inP[0], inP[1])(0, 0) / den);
  }

  /* Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override
  {
    EvaluationImplementation::save(adv);
    adv.saveAttribute("covModel_", covModel_);
    adv.saveAttribute("inputDimension_", inputDimension_);
    adv.saveAttribute("outputDimension_", outputDimension_);
  }
  /* Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override
  {
    EvaluationImplementation::load(adv);
    adv.loadAttribute("covModel_", covModel_);
    adv.loadAttribute("inputDimension_", inputDimension_);
    adv.loadAttribute("outputDimension_", outputDimension_);
  }

private:
  OT::CovarianceModel covModel_;
  OT::UnsignedInteger inputDimension_;
  OT::UnsignedInteger outputDimension_;
};


class PERSALYS_BASE_API FieldMonteCarloAnalysis : public SimulationAnalysis, public WithStopCriteriaAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  FieldMonteCarloAnalysis();
  /** Constructor with parameters */
  FieldMonteCarloAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  FieldMonteCarloAnalysis * clone() const override;

  /* Threshold accessors */
  OT::Scalar getKarhunenLoeveThreshold() const;
  void setKarhunenLoeveThreshold(const OT::Scalar threshold);

  /* Quantile probability accessors */
  OT::Scalar getQuantileLevel() const;
  void setQuantileLevel(const OT::Scalar proba);

  FieldMonteCarloResult getResult() const;

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

private:
  OT::Scalar karhunenLoeveThreshold_;
  OT::Scalar quantileLevel_;
  FieldMonteCarloResult result_;
};
}
#endif
