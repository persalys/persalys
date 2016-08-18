//                                               -*- C++ -*-
/**
 *  @brief Results of a data analysis
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
class OTGUI_API DataAnalysisResult : public OT::PersistentObject, public DataSample
{
  CLASSNAME;

public:
  typedef OT::Collection<OT::NumericalPoint> NumericalPointCollection;
  typedef OT::PersistentCollection<OT::Distribution> DistributionCollection;

  friend class DataAnalysis;

  /** Default constructor */
  DataAnalysisResult();
  /** Constructor with parameters */
  DataAnalysisResult(const OT::NumericalSample & inputSample,
                     const OT::NumericalSample & outputSample);

  /** Virtual constructor */
  virtual DataAnalysisResult * clone() const;

  OT::NumericalPoint getMin() const;
  OT::NumericalPoint getMax() const;

  OT::NumericalPoint getMean() const;
  OT::NumericalPoint getCoefficientOfVariation() const;
  OT::NumericalPoint getMedian() const;
  OT::NumericalPoint getStandardDeviation() const;
  OT::NumericalPoint getVariance() const;
  OT::NumericalPoint getSkewness() const;
  OT::NumericalPoint getKurtosis() const;
  OT::NumericalPoint getFirstQuartile() const;
  OT::NumericalPoint getThirdQuartile() const;
  OT::Interval getMeanConfidenceInterval() const;
  OT::Interval getStdConfidenceInterval() const;
  NumericalPointCollection getOutliers() const;

  DataSample::NumericalSampleCollection getPDF() const;
  DataSample::NumericalSampleCollection getCDF() const;

  double getElapsedTime() const;
  void setElapsedTime(const double seconds);

  bool isValid() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::NumericalPoint min_;
  OT::NumericalPoint max_;
  OT::NumericalPoint mean_;
  OT::NumericalPoint coefficientOfVariation_;
  OT::NumericalPoint median_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
  OT::NumericalPoint skewness_;
  OT::NumericalPoint kurtosis_;
  OT::NumericalPoint firstQuartile_;
  OT::NumericalPoint thirdQuartile_;
  OT::Interval meanConfidenceInterval_;
  OT::Interval stdConfidenceInterval_;
  OT::PersistentCollection<OT::NumericalPoint> outliers_;
  OT::PersistentCollection<OT::NumericalSample> pdf_;
  OT::PersistentCollection<OT::NumericalSample> cdf_;
  double elapsedTime_;
};
}
#endif