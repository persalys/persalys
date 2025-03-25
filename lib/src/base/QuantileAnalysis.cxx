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
#include "persalys/QuantileAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/KernelSmoothing.hxx>
#include <openturns/Normal.hxx>
#include <openturns/DistFunc.hxx>
#include <openturns/GeneralizedParetoFactory.hxx>
#include <openturns/GeneralizedPareto.hxx>
#include <openturns/FittingTest.hxx>
#include <openturns/Wilks.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/Curve.hxx>
#include <openturns/SymbolicFunction.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/Brent.hxx>

using namespace OT;

namespace PERSALYS
{

  CLASSNAMEINIT(QuantileAnalysis)

  static Factory<QuantileAnalysis> Factory_QuantileAnalysis;

  /* Default constructor */
  QuantileAnalysis::QuantileAnalysis()
    : DesignOfExperimentAnalysis()
  {

  }

  QuantileAnalysis::QuantileAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
    : DesignOfExperimentAnalysis(name, designOfExperiment)
    , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
  {
    const Sample sample = designOfExperiment_.getSample();
    defaultTargetProba_ = Point(1, 1e-3);
    if (sample.getSize())
    {
      setInterestVariables(sample.getDescription());

      // default threshold computed based on the 5% and 95% quantile
      cdfThreshold_ = Sample(2, sample.getDimension());
      cdfThreshold_.setDescription(sample.getDescription());
      cdfThreshold_[0] = Point(sample.getDimension(), 0.05);
      cdfThreshold_[1] = Point(sample.getDimension(), 0.95);

      threshold_ = Sample(2, sample.getDimension());
      threshold_.setDescription(sample.getDescription());
      threshold_[0] = sample.computeQuantilePerComponent(0.05);
      threshold_[1] = sample.computeQuantilePerComponent(0.95);
      for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
      {
        tailTypes_.add(QuantileAnalysisResult::Upper);
        targetProbas_.add(defaultTargetProba_);
      }
    }
  }


  QuantileAnalysis* QuantileAnalysis::clone() const
  {
    return new QuantileAnalysis(*this);
  }

  void QuantileAnalysis::initialize()
  {
    AnalysisImplementation::initialize();
    result_ = QuantileAnalysisResult(designOfExperiment_);
  }


  Graph QuantileAnalysis::plotMeanExcess(int iMarg, int iTail)
  {
    const std::pair<int,int> mapPair = std::make_pair(iMarg, iTail);
    if (meanExcessGraphMap_[mapPair].getDrawables().getSize() == 0)
    {
      GeneralizedParetoFactory factory;
      const Sample sampleMarg = designOfExperiment_.getSample().getMarginal(getInterestVariables()).getMarginal(iMarg);
      //upper
      if (iTail)
        meanExcessGraphMap_[mapPair] = factory.drawMeanResidualLife(sampleMarg);
      else
      {
        // reverse for MRL estimation
        Sample sample = Sample(sampleMarg.getSize(), sampleMarg.getDimension());
        for (UnsignedInteger i = 0; i < sampleMarg.getSize(); ++i)
          sample[i] = -1. * sampleMarg[i];
        const Graph graph = factory.drawMeanResidualLife(sample);
        // re-reverse for plotting
        Graph newGraph = Graph(graph);
        for (UnsignedInteger i = 0; i < graph.getDrawables().getSize(); ++i)
        {
          Sample data = -1. * graph.getDrawable(i).getData().getMarginal(0);
          data.stack(graph.getDrawable(i).getData().getMarginal(1));
          newGraph.setDrawable(Curve(data, graph.getDrawable(i).getLegend()), i);
        }
        meanExcessGraphMap_[mapPair] = newGraph;
      }
    }
    return meanExcessGraphMap_[mapPair];
  }

  Graph QuantileAnalysis::plotGPD(int iMarg, int iTail, Scalar minProba)
  {
    const std::pair<int,int> mapPair = std::make_pair(iMarg, iTail);
    const Sample xData = gpdMap_[mapPair].drawCDF().getDrawable(0).getData().getMarginal(0);
    Sample cleanedData(0, 2);
    // Upper
    if (iTail)
    {
      for (UnsignedInteger i=0; i < xData.getSize(); ++i)
      {
        Point point(2);
        point[0] = xData(i, 0);
        point[1] = gpdMap_[mapPair].computeComplementaryCDF( xData(i, 0) ) * (1 - cdfThreshold_(1, iMarg));
        if (point[1] >= minProba && point[0] >= threshold_(1, iMarg))
          cleanedData.add(point);
      }
    }
    // Lower
    else
    {
      for (UnsignedInteger i=0; i < xData.getSize(); ++i)
      {
        Point point(2);
        point[0] = - 1.0 * xData(i, 0);
        point[1] = gpdMap_[mapPair].computeComplementaryCDF( xData(i, 0) ) * (cdfThreshold_(0, iMarg));
        if (point[1] >= minProba && point[0] <= threshold_(0, iMarg))
          cleanedData.add(point);
      }
    }
    Graph graph;
    graph.add(Curve(cleanedData));
    return graph;

  }


  void QuantileAnalysis::computeCDFThreshold()
  {
    cdfThreshold_ = Sample(threshold_.getSize(), threshold_.getDimension());
    cdfThreshold_.setDescription(getInterestVariables());
    const Sample sample = designOfExperiment_.getSample().getMarginal(getInterestVariables());
    for (UnsignedInteger iMarg=0; iMarg<sample.getDimension(); ++iMarg)
    {
      cdfThreshold_(0, iMarg) = sample.getMarginal(iMarg).computeEmpiricalCDF(Point(1, threshold_(0, iMarg)));
      cdfThreshold_(1, iMarg) = sample.getMarginal(iMarg).computeEmpiricalCDF(Point(1, threshold_(1, iMarg)));
    }
  }

  void QuantileAnalysis::setThreshold(const Sample& threshold)
  {
    if (threshold.getSize() != 2)
      throw InvalidArgumentException(HERE) << "Threshold sample size must be equal to 2.";
    if (threshold.getDimension() != getInterestVariables().getSize())
      throw InvalidArgumentException(HERE) << "Threshold dimension must match the number of interest variables.";

    threshold_ = threshold;
    threshold_.setDescription(getInterestVariables());
    computeCDFThreshold();
  }

  void QuantileAnalysis::checkThresholdCompatibility() const
  {
    for (UnsignedInteger iMarg=0; iMarg<cdfThreshold_.getMarginal(getInterestVariables()).getDimension(); ++iMarg)
    {
      // lower tail
      if ((tailTypes_[iMarg] & QuantileAnalysisResult::Lower || tailTypes_[iMarg] & QuantileAnalysisResult::Bilateral) && targetProbas_[iMarg].normInf() >= cdfThreshold_(0, iMarg))
        throw InvalidArgumentException(HERE) << "The maximum target probability for " << cdfThreshold_.getDescription()[iMarg] << " is not suitable with the threshold of the lower tail : it should be lower than " << cdfThreshold_(0, iMarg);
      // upper tail
      if ((tailTypes_[iMarg] & QuantileAnalysisResult::Upper || tailTypes_[iMarg] & QuantileAnalysisResult::Bilateral) && targetProbas_[iMarg].normInf() >= 1 - cdfThreshold_(1, iMarg))
        throw InvalidArgumentException(HERE) << "The maximum target probability for " << cdfThreshold_.getDescription()[iMarg] << " is not suitable with the threshold of the upper tail : it should be lower than " << 1 - cdfThreshold_(1, iMarg);
    }
  }

  void QuantileAnalysis::launch()
  {
    const Sample sample = getDesignOfExperiment().getSample().getMarginal(getInterestVariables());
    if (!sample.getSize())
      throw InvalidDimensionException(HERE) << "The sample is empty";

    if (sample.getDimension() != tailTypes_.getSize())
      throw InvalidDimensionException(HERE) << "Each interest variable must be associated to a tail type";
    if (sample.getDimension() != targetProbas_.getSize())
      throw InvalidDimensionException(HERE) << "Each interest variable must be associated to at least one target probability";

    switch (type_)
    {
    case QuantileAnalysisResult::MonteCarlo:
      result_.type_ = QuantileAnalysisResult::MonteCarlo;
      launchMonteCarlo(sample);
      break;
    case QuantileAnalysisResult::GeneralizedPareto:
      result_.type_ = QuantileAnalysisResult::GeneralizedPareto;
      launchGeneralizedPareto(sample, threshold_.getMarginal(getInterestVariables()), cdfThreshold_.getMarginal(getInterestVariables()));
      break;
    default:
      throw InvalidArgumentException(HERE) << "Unknown quantile analysis type\n";
    }
  }

  Indices QuantileAnalysis::computeWilksValidity(const Point & probaValues, const UnsignedInteger & sampleSize, const QuantileAnalysisResult::TailType & tail) const
  {
    // TODO: use QuantileConfidence::computeUnilateralMinimumSampleSize/computeBilateralMinimumSampleSize
    Indices wilks(probaValues.getDimension());
    for (UnsignedInteger iProba=0; iProba<probaValues.getDimension(); ++iProba)
    {
      Scalar nApprox = 0.0;
      if (tail & QuantileAnalysisResult::Lower || tail & QuantileAnalysisResult::Upper)
        nApprox = std::max(std::ceil(std::log1p(-ciLevel_) / std::log1p(-probaValues[iProba])), nApprox);
      if (tail & QuantileAnalysisResult::Bilateral)
      {
        const Scalar gamma = std::max(probaValues[iProba], 1.0 - probaValues[iProba]);
        const Scalar nMin = std::ceil(std::log1p(-ciLevel_) / std::log(gamma));
        const Scalar nMax = std::ceil((std::log1p(-ciLevel_) - std::log(2)) / std::log(gamma));
        const SymbolicFunction residualFunction(Description({"n", "alpha", "beta"}), Description({"1 - alpha^n - (1 - alpha)^n - beta"}));
        const ParametricFunction residualParametric(residualFunction, Indices({1, 2}), Point({probaValues[iProba], ciLevel_}));
        Brent solver;
        const Scalar root = solver.solve(residualParametric, 0.0, nMin - 1, nMax);
        nApprox = std::max(std::ceil(root), nApprox);
      }
      wilks[iProba] = (int)(std::ceil(nApprox) <= sampleSize);
    }
    return wilks;
  }

  void QuantileAnalysis::launchMonteCarlo(const Sample & sample)
  {
    const UnsignedInteger sampleSize = sample.getSize();

    for (UnsignedInteger iMarg=0; iMarg<sample.getDimension(); ++iMarg)
    {
      if (stopRequested_)
        break;
      PersistentCollection<Sample> quantilesMarg;
      progressValue_ = (int) (iMarg * 100 / sample.getDimension());
      notify("progressValueChanged");

      const Distribution dist = KernelSmoothing(Normal()).build(sample.getMarginal(iMarg));
      Sample probaValues(0, targetProbas_[iMarg].getDimension());
      if (tailTypes_[iMarg] & QuantileAnalysisResult::Lower)
      {
        probaValues.add(targetProbas_[iMarg]);
        result_.wilksValidity_[std::make_pair(sample.getDescription()[iMarg], QuantileAnalysisResult::Lower)] = computeWilksValidity(targetProbas_[iMarg], sampleSize, QuantileAnalysisResult::Lower);
      }
      if (tailTypes_[iMarg] & QuantileAnalysisResult::Upper)
      {
        probaValues.add(Point(targetProbas_[iMarg].getDimension(), 1.) - targetProbas_[iMarg]);
        result_.wilksValidity_[std::make_pair(sample.getDescription()[iMarg], QuantileAnalysisResult::Upper)] = computeWilksValidity(targetProbas_[iMarg], sampleSize, QuantileAnalysisResult::Upper);
      }
      if (tailTypes_[iMarg] & QuantileAnalysisResult::Bilateral)
      {
        probaValues.add(0.5 * targetProbas_[iMarg]);
        probaValues.add(Point(targetProbas_[iMarg].getDimension(), 1.) - 0.5 * targetProbas_[iMarg]);
        result_.wilksValidity_[std::make_pair(sample.getDescription()[iMarg], QuantileAnalysisResult::Bilateral)] = computeWilksValidity(targetProbas_[iMarg], sampleSize, QuantileAnalysisResult::Bilateral);
      }

      for (UnsignedInteger iTail=0; iTail<probaValues.getSize(); ++iTail)
      {
        Sample quantile = Sample(targetProbas_[iMarg].getDimension(), 3);
        for (UnsignedInteger iProba=0; iProba<targetProbas_[iMarg].getDimension(); ++iProba)
        {
          const Scalar q = sample.getMarginal(iMarg).computeQuantile(probaValues(iTail, iProba))[0];
          const Scalar s = std::sqrt(probaValues(iTail, iProba) * (1 - probaValues(iTail, iProba)) / sampleSize) / dist.computePDF(q);
          quantile(iProba, 0) = q + s * DistFunc::qNormal(0.5 * (1. - ciLevel_));
          quantile(iProba, 1) = q;
          quantile(iProba, 2) = q + s * DistFunc::qNormal(1 - 0.5 * (1. - ciLevel_));
        }
        quantilesMarg.add(quantile);
      }
      // Store for each marginal
      result_.quantiles_[sample.getDescription()[iMarg]] = quantilesMarg;
    }
  }

  void QuantileAnalysis::launchGeneralizedPareto(const Sample & sample, const Sample & threshold, const Sample & cdfThreshold)
  {
    checkThresholdCompatibility();
    for (UnsignedInteger iMarg=0; iMarg<sample.getDimension(); ++iMarg)
    {
      if (stopRequested_)
        break;
      progressValue_ = (int) (iMarg * 100 / sample.getDimension());
      notify("progressValueChanged");

      RandomGenerator::SetSeed(getSeed());

      // build the list of proba values
      Sample sampleMarg = sample.getMarginal(iMarg);
      PersistentCollection<Sample> quantilesMarg;
      PersistentCollection<Point> validityMarg;
      Sample probaValues(0, targetProbas_[iMarg].getDimension());
      Indices fullTailTypes;
      if (tailTypes_[iMarg] & QuantileAnalysisResult::Lower)
      {
        probaValues.add(targetProbas_[iMarg] / cdfThreshold(0, iMarg));
        fullTailTypes.add(QuantileAnalysisResult::Lower);
      }
      if (tailTypes_[iMarg] & QuantileAnalysisResult::Upper)
      {
        probaValues.add(targetProbas_[iMarg] / (1 - cdfThreshold(1, iMarg)));
        fullTailTypes.add(QuantileAnalysisResult::Upper);
      }
      if (tailTypes_[iMarg] & QuantileAnalysisResult::Bilateral)
      {
        probaValues.add(0.5 * targetProbas_[iMarg] / cdfThreshold(0, iMarg));
        fullTailTypes.add(QuantileAnalysisResult::Lower);
        probaValues.add(0.5 * targetProbas_[iMarg] / (1 - cdfThreshold(1, iMarg)));
        fullTailTypes.add(QuantileAnalysisResult::Upper);
      }

      /* build the estimator only once for lower and upper if needed
         use the same parametersSample for all proba values to get coherent bounds
      */
      Distribution distributionLower = GeneralizedPareto();
      Sample parametersSampleLower;
      Scalar pvalueLower(0);
      Distribution distributionUpper = GeneralizedPareto();
      Sample parametersSampleUpper;
      Scalar pvalueUpper(0);
      if ((tailTypes_[iMarg] & QuantileAnalysisResult::Lower) | (tailTypes_[iMarg] & QuantileAnalysisResult::Bilateral))
      {
        DistributionFactoryLikelihoodResult estimatorLower = GeneralizedParetoFactory().buildMethodOfLikelihoodMaximizationEstimator(-1. * sampleMarg, -1 * threshold(0, iMarg));
        distributionLower = estimatorLower.getDistribution();
        const std::pair<int,int> mapPair = std::make_pair(iMarg, 0);
        gpdMap_[mapPair] = distributionLower;
        parametersSampleLower = estimatorLower.getParameterDistribution().getSample(paramSampleSize_);
        Sample peaksSampleLower(0, 1);
        for (UnsignedInteger m=0; m<sampleMarg.getSize(); ++m)
        {
          if (sampleMarg(m, 0) < threshold(0, iMarg))
            peaksSampleLower.add(-1. * sampleMarg[m]);
        }
        pvalueLower = FittingTest::Kolmogorov(peaksSampleLower, distributionLower).getPValue();
        result_.pValue_[std::make_pair(sample.getDescription()[iMarg], QuantileAnalysisResult::Lower)] = pvalueLower;
      }
      if ((tailTypes_[iMarg] & QuantileAnalysisResult::Upper) | (tailTypes_[iMarg] & QuantileAnalysisResult::Bilateral))
      {
        DistributionFactoryLikelihoodResult estimatorUpper = GeneralizedParetoFactory().buildMethodOfLikelihoodMaximizationEstimator(sampleMarg, threshold(1, iMarg));
        distributionUpper = estimatorUpper.getDistribution();
        const std::pair<int,int> mapPair = std::make_pair(iMarg, 1);
        gpdMap_[mapPair] = distributionUpper;
        parametersSampleUpper = estimatorUpper.getParameterDistribution().getSample(paramSampleSize_);
        Sample peaksSampleUpper(0, 1);
        for (UnsignedInteger m=0; m<sampleMarg.getSize(); ++m)
        {
          if (sampleMarg(m, 0) > threshold(1, iMarg))
            peaksSampleUpper.add(sampleMarg[m]);
        }
        pvalueUpper = FittingTest::Kolmogorov(peaksSampleUpper, distributionUpper).getPValue();
        result_.pValue_[std::make_pair(sample.getDescription()[iMarg], QuantileAnalysisResult::Upper)] = pvalueUpper;
      }

      // compute quantile for all proba values
      for (UnsignedInteger iTail=0; iTail<probaValues.getSize(); ++iTail)
      {
        // select the proper distribution and pvalue
        Sample parametersSample;
        Distribution distribution = GeneralizedPareto();
        Scalar pvalue;
        Scalar coefTail;
        switch (fullTailTypes[iTail])
        {
        case QuantileAnalysisResult::Lower:
          parametersSample = parametersSampleLower;
          distribution = distributionLower;
          pvalue = pvalueLower;
          coefTail = -1;
          break;
        case QuantileAnalysisResult::Upper:
          parametersSample = parametersSampleUpper;
          distribution = distributionUpper;
          pvalue = pvalueUpper;
          coefTail = 1;
          break;
        default:
          throw InvalidArgumentException(HERE) << "Unknown tail type\n";
        }

        // compute the confidence interval for all proba values
        Sample pareto_quantiles = Sample(0, 1);
        Distribution distributionParametric = GeneralizedPareto();
        Sample quantile = Sample(targetProbas_[iMarg].getDimension(), 3);
        Point validity = Point(targetProbas_[iMarg].getDimension());
        for (UnsignedInteger iProba=0; iProba<targetProbas_[iMarg].getDimension(); ++iProba)
        {
          for (UnsignedInteger l=0; l<parametersSample.getSize(); ++l)
          {
            try
            {
              // accept failure of some quantile evaluations because of sigma < 0
              distributionParametric.setParameter(parametersSample[l]);
              pareto_quantiles.add(coefTail * distributionParametric.computeQuantile(probaValues(iTail, iProba), true));
            }
            catch (Exception &)
            {
              // pass
            }
          }
          const Scalar error = (1.0 * paramSampleSize_ - pareto_quantiles.getSize()) / paramSampleSize_;
          if (error > samplingErrorTolerance_)
          {
            throw NotDefinedException(HERE) << "Too much parameter samples errored (" << error << ") for " << sample.getDescription()[iMarg] << " with tail (" << fullTailTypes[iTail] << ") and target probability (" << targetProbas_[iMarg][iProba] << ") in launchGeneralizedPareto.";
          }
          else if (error > 0.0)
            LOGWARN(OSS() << "Some parameter samples errored (" << error << ") for " << sample.getDescription()[iMarg] << " with tail (" << fullTailTypes[iTail] << ") and target probability (" << targetProbas_[iMarg][iProba] << ") in launchGeneralizedPareto");
          quantile(iProba, 0) = pareto_quantiles.computeQuantile((1. - ciLevel_) / 2.)[0];
          quantile(iProba, 1) = coefTail * distribution.computeQuantile(probaValues(iTail, iProba), true)[0];
          quantile(iProba, 2) = pareto_quantiles.computeQuantile(1. - (1. - ciLevel_) / 2.)[0];
          validity[iProba] = pvalue;
        }
        quantilesMarg.add(quantile);
        validityMarg.add(validity);

      }
      // Store for each marginal
      result_.quantiles_[sample.getDescription()[iMarg]] = quantilesMarg;
    }
  }

  QuantileAnalysisResult QuantileAnalysis::getResult() const
  {
    return result_;
  }


  String QuantileAnalysis::getPythonScript() const
  {
    OSS oss;
    oss << getName() << " = persalys.QuantileAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";
    oss << getName() << ".setInterestVariables(" << Parameters::GetOTDescriptionStr(getInterestVariables()) << ")\n";
    oss << getName() << ".setTargetProbabilities([";
    for (UnsignedInteger i=0; i<getTargetProbabilities().getSize(); ++i)
    {
      oss << Parameters::GetOTPointStr(getTargetProbabilities()[i]);
      if (i != getTargetProbabilities().getSize() - 1)
        oss << ",";
    }
    oss << "])\n";
    oss << getName() << ".setTailTypes(" << Parameters::GetOTIndicesStr(getTailTypes()) << ")\n";
    oss << getName() << ".setType(" << getType() << ")\n";
    oss << getName() << ".setThreshold(" << Parameters::GetOTSampleStr(getThreshold()) << ")\n";
    return oss;
  }


  bool QuantileAnalysis::hasValidResult() const
  {
    return getResult().getSize() != 0;
  }


  String QuantileAnalysis::__repr__() const
  {
    OSS oss;
    oss << "class=" << GetClassName()
        << " name=" << getName()
        << " designOfExperiment=" << getDesignOfExperiment().getName()
        << " tailTypes=" << getTailTypes().__str__()
        << " targetProbabilities=" << getTargetProbabilities().__str__();

    return oss;
  }


  void QuantileAnalysis::save(Advocate & adv) const
  {
    DesignOfExperimentAnalysis::save(adv);
    adv.saveAttribute("result_", result_);
    adv.saveAttribute("type_", type_);
    adv.saveAttribute("targetProbas_", targetProbas_);
    adv.saveAttribute("defaultTargetProba_", defaultTargetProba_);
    adv.saveAttribute("tailTypes_", tailTypes_);
    adv.saveAttribute("ciLevel_", ciLevel_);
    adv.saveAttribute("threshold_", threshold_);
    adv.saveAttribute("cdfThreshold_", cdfThreshold_);
    adv.saveAttribute("paramSampleSize_", paramSampleSize_);
    adv.saveAttribute("seed_", seed_);
    adv.saveAttribute("samplingErrorTolerance_", samplingErrorTolerance_);
    OT::Indices keys1;
    OT::Indices keys2;
    OT::PersistentCollection<OT::Distribution> gpdMap;
    for (auto const& val : gpdMap_)
    {
      keys1.add(val.first.first);
      keys2.add(val.first.second);
      gpdMap.add(val.second);
    }
    adv.saveAttribute("keys1_", keys1);
    adv.saveAttribute("keys2_", keys2);
    adv.saveAttribute("gpdMap_", gpdMap);
  }


  void QuantileAnalysis::load(Advocate & adv)
  {
    DesignOfExperimentAnalysis::load(adv);
    adv.loadAttribute("result_", result_);
    adv.loadAttribute("type_", type_);
    adv.loadAttribute("targetProbas_", targetProbas_);
    adv.loadAttribute("defaultTargetProba_", defaultTargetProba_);
    adv.loadAttribute("tailTypes_", tailTypes_);
    adv.loadAttribute("ciLevel_", ciLevel_);
    adv.loadAttribute("threshold_", threshold_);
    adv.loadAttribute("cdfThreshold_", cdfThreshold_);
    adv.loadAttribute("paramSampleSize_", paramSampleSize_);
    adv.loadAttribute("seed_", seed_);
    adv.loadAttribute("samplingErrorTolerance_", samplingErrorTolerance_);

    OT::Indices keys1;
    OT::Indices keys2;
    OT::PersistentCollection<OT::Distribution> gpdMap;
    adv.loadAttribute("keys1_", keys1);
    adv.loadAttribute("keys2_", keys2);
    adv.loadAttribute("gpdMap_", gpdMap);
    for (UnsignedInteger i=0; i<keys1.getSize(); ++i)
      gpdMap_[std::make_pair(keys1[i], keys2[i])] = gpdMap[i];
  }



}
