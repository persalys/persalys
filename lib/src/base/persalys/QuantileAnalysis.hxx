//                                               -*- C++ -*-
/**
 *  @brief Quantile analysis
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

#ifndef PERSALYS_QUANTILEANALYSIS_HXX
#define PERSALYS_QUANTILEANALYSIS_HXX

#include "DesignOfExperimentAnalysis.hxx"
#include "QuantileAnalysisResult.hxx"

namespace PERSALYS
{
  class PERSALYS_BASE_API QuantileAnalysis : public DesignOfExperimentAnalysis
  {
    CLASSNAME


  public:

    /** Default constructor */
    QuantileAnalysis();

    /** Constructor with parameters */
    QuantileAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment);

    /** Virtual constructor */
    QuantileAnalysis * clone() const override;

    void setTargetProbabilities(const OT::Collection<OT::Point>& targetProbas) {targetProbas_ = targetProbas;}
    OT::Collection<OT::Point> getTargetProbabilities() const {return targetProbas_;}

    void setDefaultTargetProbability(const OT::Point& targetProba) {defaultTargetProba_ = targetProba;}
    OT::Point getDefaultTargetProbability() const {return defaultTargetProba_;}

    void setType (const OT::UnsignedInteger& type) {type_ = type;}
    OT::UnsignedInteger getType() const {return type_;}

    void setTailTypes (const OT::Indices& tailTypes) {tailTypes_ = tailTypes;}
    OT::Indices getTailTypes() const {return tailTypes_;}

    void setThreshold (const OT::Sample& threshold);
    OT::Sample getThreshold() const {return threshold_;}

    OT::Sample getCDFThreshold() const {return cdfThreshold_;}

    void checkThresholdCompatibility() const;

    void setParameterSampleSize (const OT::Scalar& paramSampleSize) {paramSampleSize_ = paramSampleSize;}
    OT::Scalar getParameterSampleSize() const {return paramSampleSize_;}

    void setConfidenceIntervalLevel (const OT::Scalar& ciLevel)
    {
      if (ciLevel < 0 || ciLevel > 1)
        throw OT::InvalidArgumentException(HERE) << "The confidence interval level must be in [0;1].";
      ciLevel_ = ciLevel;
    }
    OT::Scalar getConfidenceIntervalLevel() const {return ciLevel_;}

    void setSeed (const OT::UnsignedInteger& seed) {seed_ = seed;}
    OT::UnsignedInteger getSeed() const {return seed_;}

    OT::String getPythonScript() const override;
    bool hasValidResult() const override;

    QuantileAnalysisResult getResult() const;

    /** String converter */
    OT::String __repr__() const override;

    /** Method save() stores the object through the StorageManager */
    void save(OT::Advocate & adv) const override;

    /** Method load() reloads the object from the StorageManager */
    void load(OT::Advocate & adv) override;

    OT::Graph plotMeanExcess(int iMarg, int iTail);
    OT::Graph plotGPD(int iMarg, int iTail, OT::Scalar minProba);

    OT::Indices computeWilksValidity(const OT::Point & probaValue, const OT::UnsignedInteger & sampleSize, const QuantileAnalysisResult::TailType & tail) const;

  protected:
    void initialize() override;
    void computeCDFThreshold();
    void launch() override;
    void launchMonteCarlo(const OT::Sample & sample);
    void launchGeneralizedPareto(const OT::Sample & sample, const OT::Sample & threshold, const OT::Sample & cdfThreshold);

  private:
    QuantileAnalysisResult result_;
    OT::UnsignedInteger type_ = QuantileAnalysisResult::MonteCarlo;
    OT::PersistentCollection<OT::Point> targetProbas_;
    OT::Point defaultTargetProba_;
    OT::Indices tailTypes_;
    OT::Scalar ciLevel_ = 0.95;
    OT::Sample threshold_;
    OT::Sample cdfThreshold_;
    OT::Scalar paramSampleSize_ = 1000;
    OT::UnsignedInteger seed_ = 0;
    OT::Scalar samplingErrorTolerance_ = 0.1;
    std::map<std::pair<int,int>, OT::Graph> meanExcessGraphMap_;
    std::map<std::pair<int,int>, OT::Distribution> gpdMap_;
  };
}
#endif
