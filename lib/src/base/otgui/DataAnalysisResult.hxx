//                                               -*- C++ -*-
/**
 *  @brief Results of a data analysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_DATAANALYSISRESULT_HXX
#define OTGUI_DATAANALYSISRESULT_HXX

#include "EvaluationResult.hxx"

#include <openturns/Distribution.hxx>

namespace OTGUI
{
class OTGUI_API DataAnalysisResult : public EvaluationResult
{
  CLASSNAME

public:
  typedef OT::Collection<OT::Point> PointCollection;
  typedef OT::PersistentCollection<OT::Distribution> DistributionCollection;

  friend class DataAnalysis;
  friend class MonteCarloAnalysis;

  /** Default constructor */
  DataAnalysisResult();

  /** Constructor with parameters */
  DataAnalysisResult(const DesignOfExperiment& design);

  /** Virtual constructor */
  virtual DataAnalysisResult * clone() const;

  PointCollection getMin() const;
  PointCollection getMax() const;

  PointCollection getMean() const;
  PointCollection getCoefficientOfVariation() const;
  PointCollection getMedian() const;
  PointCollection getStandardDeviation() const;
  PointCollection getVariance() const;
  PointCollection getSkewness() const;
  PointCollection getKurtosis() const;
  PointCollection getFirstQuartile() const;
  PointCollection getThirdQuartile() const;
  OT::Interval getMeanConfidenceInterval() const;
  OT::Interval getStdConfidenceInterval() const;
  PointCollection getOutliers() const;

  DataSample::SampleCollection getPDF() const;
  DataSample::SampleCollection getCDF() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::PersistentCollection<OT::Point> min_;
  OT::PersistentCollection<OT::Point> max_;
  OT::PersistentCollection<OT::Point> mean_;
  OT::PersistentCollection<OT::Point> coefficientOfVariation_;
  OT::PersistentCollection<OT::Point> median_;
  OT::PersistentCollection<OT::Point> standardDeviation_;
  OT::PersistentCollection<OT::Point> variance_;
  OT::PersistentCollection<OT::Point> skewness_;
  OT::PersistentCollection<OT::Point> kurtosis_;
  OT::PersistentCollection<OT::Point> firstQuartile_;
  OT::PersistentCollection<OT::Point> thirdQuartile_;
  OT::Interval meanConfidenceInterval_;
  OT::Interval stdConfidenceInterval_;
  OT::PersistentCollection<OT::Point> outliers_;
  OT::PersistentCollection<OT::Sample> pdf_;
  OT::PersistentCollection<OT::Sample> cdf_;
};
}
#endif
