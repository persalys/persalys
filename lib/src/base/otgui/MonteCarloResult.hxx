//                                               -*- C++ -*-
/**
 *  @brief Results of a Monte Carlo analysis 
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_MONTECARLORESULT_HXX
#define OTGUI_MONTECARLORESULT_HXX

#include "SimulationAnalysisResult.hxx"
#include "Distribution.hxx"

namespace OTGUI {
class MonteCarloResult : public SimulationAnalysisResult
{
  CLASSNAME;

public:

  typedef OT::Collection<OT::NumericalPoint> NumericalPointCollection;
  typedef OT::Collection<OT::Distribution> DistributionCollection;

  MonteCarloResult();
  MonteCarloResult(OT::NumericalSample inputSample, OT::NumericalSample outputSample);

  virtual MonteCarloResult * clone() const;

  OT::NumericalPoint getMean();
  OT::NumericalPoint getMedian();
  OT::NumericalPoint getStandardDeviation();
  OT::NumericalPoint getVariance();
  OT::NumericalPoint getSkewness();
  OT::NumericalPoint getKurtosis();
  OT::NumericalPoint getFirstQuartile();
  OT::NumericalPoint getThirdQuartile();
  OT::Interval getMeanConfidenceInterval(const double level=0.95);
  OT::Interval getStdConfidenceInterval(const double level=0.95);
  NumericalPointCollection getOutliers();

  DistributionCollection getFittedDistribution();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  double levelConfidenceInterval_;
  OT::NumericalPoint mean_;
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
};
}
#endif