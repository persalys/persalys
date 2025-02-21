//                                               -*- C++ -*-
/**
 *  @brief Compute the KL decomposition of a field model
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

#include "persalys/FieldKarhunenLoeveAnalysis.hxx"

#include <openturns/KarhunenLoeveSVDAlgorithm.hxx>
#include <openturns/PiecewiseLinearEvaluation.hxx>
#include <openturns/InverseTrendTransform.hxx>
#include <openturns/NonStationaryCovarianceModelFactory.hxx>
#include <openturns/PersistentObjectFactory.hxx>


using namespace OT;
namespace PERSALYS
{

  class CovFunctionEvaluation : public OT::EvaluationImplementation
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

  CLASSNAMEINIT(CovFunctionEvaluation)
  static Factory<CovFunctionEvaluation> Factory_CovFunctionEvaluation;


  CLASSNAMEINIT(FieldKarhunenLoeveAnalysis)
  static Factory<FieldKarhunenLoeveAnalysis> Factory_FieldKarhunenLoeveAnalysis;


  /* Default constructor */
  FieldKarhunenLoeveAnalysis::FieldKarhunenLoeveAnalysis()
    : AnalysisImplementation()
    , model_()
    , result_()
  {
  }

  FieldKarhunenLoeveAnalysis::FieldKarhunenLoeveAnalysis(const String & name)
    : AnalysisImplementation(name)
    , model_()
    , result_()
  {
  }

  FieldKarhunenLoeveAnalysis::FieldKarhunenLoeveAnalysis(const String & name,
                                                         const DataFieldModel& model)
    : AnalysisImplementation(name)
    , model_(model)
    , result_()
  {
  }

  /* Virtual constructor */
  FieldKarhunenLoeveAnalysis* FieldKarhunenLoeveAnalysis::clone() const
  {
    return new FieldKarhunenLoeveAnalysis(*this);
  }


  Observer * FieldKarhunenLoeveAnalysis::getParentObserver() const
  {
    return model_.getImplementation()->getObserver("DataFieldModelDiagramItem");
  }

  void FieldKarhunenLoeveAnalysis::launch()
  {
    result_.processSample_ = model_.getProcessSample();
    Mesh mesh = result_.processSample_.getMesh();

    if (!mesh.getVertices().getSize())
      throw InvalidArgumentException(HERE) << "FieldKarhuneneLoeveAnalysis: cannot find mesh data";

    // Compute mean/quantiles process sample
    result_.meanSample_ = result_.processSample_.computeMean().getValues();
    result_.lowerQuantileSample_ = result_.processSample_.computeQuantilePerComponent(quantileLevel_).getValues();
    result_.upperQuantileSample_ = result_.processSample_.computeQuantilePerComponent(1 - quantileLevel_).getValues();

    // Compute the KL decomposition of the output and correlation
    informationMessage_ = "Karhunen-Loeve algorithm in progress";
    notify("informationMessageUpdated");
    // get mesh info
    const Point vertices(mesh.getVertices().asPoint());
    try
    {
      for (UnsignedInteger i = 0; i < result_.processSample_.getDimension(); ++i)
      {
        const ProcessSample ps_i(result_.processSample_.getMarginal(i));
        // Compute the KL decomposition
        KarhunenLoeveSVDAlgorithm algoKL(ps_i, getKarhunenLoeveThreshold());
        algoKL.run();
        result_.karhunenLoeveResults_.add(algoKL.getResult());
        // compute correlation
        const TrendTransform meanTransform(PiecewiseLinearEvaluation(vertices, result_.meanSample_.getMarginal(i)), mesh);
        const InverseTrendTransform meanInverseTransform(meanTransform.getInverse());
        const ProcessSample sample_centered(meanInverseTransform(ps_i));

        CovarianceModel covariance(NonStationaryCovarianceModelFactory().build(sample_centered, true));
        Function f(new CovFunctionEvaluation(covariance));
        result_.correlationFunction_.add(f);

        Sample xi_sample(algoKL.getResult().project(sample_centered));
        xi_sample.setDescription(Description::BuildDefault(algoKL.getResult().getEigenvalues().getSize(), "Xi_"));
        result_.xiSamples_.add(xi_sample);
      }
    }
    catch (std::exception & ex)
    {
      warningMessage_ = ex.what();
    }
  }


  Scalar FieldKarhunenLoeveAnalysis::getKarhunenLoeveThreshold() const
  {
    return karhunenLoeveThreshold_;
  }


  void FieldKarhunenLoeveAnalysis::setKarhunenLoeveThreshold(const Scalar threshold)
  {
    karhunenLoeveThreshold_ = threshold;
  }


  Scalar FieldKarhunenLoeveAnalysis::getQuantileLevel() const
  {
    return quantileLevel_;
  }

  bool FieldKarhunenLoeveAnalysis::hasValidResult() const
  {
    return getResult().getXiSamples().getSize() != 0;
  }

  void FieldKarhunenLoeveAnalysis::setQuantileLevel(const Scalar level)
  {
    if (!(level >= 0.0) || !(level <= 1.0))
      throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
    quantileLevel_ = level;
  }


  FieldMonteCarloResult FieldKarhunenLoeveAnalysis::getResult() const
  {
    return result_;
  }


  String FieldKarhunenLoeveAnalysis::getPythonScript() const
  {
    OSS oss;
    oss.setPrecision(12);
    oss << getName() << " = persalys.FieldKarhunenLoeveAnalysis('" << getName() << "', " << getDataFieldModel().getName() << ")\n";
    oss << getName() << ".setKarhunenLoeveThreshold(" << getKarhunenLoeveThreshold() << ")\n";
    oss << getName() << ".setQuantileLevel(" << getQuantileLevel() << ")\n";

    return oss;
  }


  String FieldKarhunenLoeveAnalysis::__repr__() const
  {
    OSS oss;
    oss << " Karhunen-Loeve threshold=" << getKarhunenLoeveThreshold()
        << " quantile level=" << getQuantileLevel();
    return oss;
  }

  void FieldKarhunenLoeveAnalysis::save(Advocate & adv) const
  {
   AnalysisImplementation::save(adv);
   adv.saveAttribute("model_", model_);
   adv.saveAttribute("karhunenLoeveThreshold_", karhunenLoeveThreshold_);
   adv.saveAttribute("quantileLevel_", quantileLevel_);
   adv.saveAttribute("result_", result_);
  }


  void FieldKarhunenLoeveAnalysis::load(Advocate & adv)
  {
   AnalysisImplementation::load(adv);
   adv.loadAttribute("model_", model_);
   adv.loadAttribute("karhunenLoeveThreshold_", karhunenLoeveThreshold_);
   adv.loadAttribute("quantileLevel_", quantileLevel_);
   adv.loadAttribute("result_", result_);
  }

}
