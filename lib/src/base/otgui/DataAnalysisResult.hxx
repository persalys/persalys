//                                               -*- C++ -*-
/**
 *  @brief Results of a data analysis
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_DATAANALYSISRESULT_HXX
#define OTGUI_DATAANALYSISRESULT_HXX

#include "DataSample.hxx"
#include "otgui/OTGuiprivate.hxx"

#include "openturns/OTType.hxx"
#include "openturns/Distribution.hxx"

namespace OTGUI {
class OTGUI_API DataAnalysisResult : public DataSample
{
  CLASSNAME;

public:
  typedef OT::Collection<OT::NumericalPoint> NumericalPointCollection;
  typedef OT::PersistentCollection<OT::Distribution> DistributionCollection;

  friend class DataAnalysis;
  friend class MonteCarloAnalysis;

  /** Default constructor */
  DataAnalysisResult();
  /** Constructor with parameters */
  DataAnalysisResult(const OT::NumericalSample & inputSample,
                     const OT::NumericalSample & outputSample);

  /** Virtual constructor */
  virtual DataAnalysisResult * clone() const;

  NumericalPointCollection getMin() const;
  NumericalPointCollection getMax() const;

  NumericalPointCollection getMean() const;
  NumericalPointCollection getCoefficientOfVariation() const;
  NumericalPointCollection getMedian() const;
  NumericalPointCollection getStandardDeviation() const;
  NumericalPointCollection getVariance() const;
  NumericalPointCollection getSkewness() const;
  NumericalPointCollection getKurtosis() const;
  NumericalPointCollection getFirstQuartile() const;
  NumericalPointCollection getThirdQuartile() const;
  OT::Interval getMeanConfidenceInterval() const;
  OT::Interval getStdConfidenceInterval() const;
  NumericalPointCollection getOutliers() const;

  DataSample::NumericalSampleCollection getPDF() const;
  DataSample::NumericalSampleCollection getCDF() const;

  double getElapsedTime() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::PersistentCollection<OT::NumericalPoint> min_;
  OT::PersistentCollection<OT::NumericalPoint> max_;
  OT::PersistentCollection<OT::NumericalPoint> mean_;
  OT::PersistentCollection<OT::NumericalPoint> coefficientOfVariation_;
  OT::PersistentCollection<OT::NumericalPoint> median_;
  OT::PersistentCollection<OT::NumericalPoint> standardDeviation_;
  OT::PersistentCollection<OT::NumericalPoint> variance_;
  OT::PersistentCollection<OT::NumericalPoint> skewness_;
  OT::PersistentCollection<OT::NumericalPoint> kurtosis_;
  OT::PersistentCollection<OT::NumericalPoint> firstQuartile_;
  OT::PersistentCollection<OT::NumericalPoint> thirdQuartile_;
  OT::Interval meanConfidenceInterval_;
  OT::Interval stdConfidenceInterval_;
  OT::PersistentCollection<OT::NumericalPoint> outliers_;
  OT::PersistentCollection<OT::NumericalSample> pdf_;
  OT::PersistentCollection<OT::NumericalSample> cdf_;
  double elapsedTime_;
};
}
#endif