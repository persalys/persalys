//                                               -*- C++ -*-
/**
 *  @brief Class for analysis with stop criteria
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
#include "persalys/WithStopCriteriaAnalysis.hxx"

#include "persalys/AnalysisImplementation.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

/* Default constructor */
WithStopCriteriaAnalysis::WithStopCriteriaAnalysis()
  : maximumCalls_(std::numeric_limits<int>::max())
  , maximumCoefficientOfVariation_(0.01)
  , maximumElapsedTime_(60) // in seconds
{
}


WithStopCriteriaAnalysis::~WithStopCriteriaAnalysis()
{
}


bool WithStopCriteriaAnalysis::Stop(void * p)
{
  TimeCriteria * arg = (TimeCriteria*)p;
  arg->incrementElapsedTime();
  return arg->stopRequested_ || (arg->elapsedTime_ > arg->maximumElapsedTime_);
}


UnsignedInteger WithStopCriteriaAnalysis::getMaximumCalls() const
{
  return maximumCalls_;
}


void WithStopCriteriaAnalysis::setMaximumCalls(const UnsignedInteger maxi)
{
  if (maxi < 1)
    throw InvalidValueException(HERE) << "The maximum calls number cannot be null";

  maximumCalls_ = maxi;
}


Scalar WithStopCriteriaAnalysis::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}


void WithStopCriteriaAnalysis::setMaximumCoefficientOfVariation(const Scalar coef)
{
  maximumCoefficientOfVariation_ = coef;
}


Scalar WithStopCriteriaAnalysis::getMaximumElapsedTime() const
{
  return maximumElapsedTime_;
}


void WithStopCriteriaAnalysis::setMaximumElapsedTime(const Scalar seconds)
{
  maximumElapsedTime_ = seconds;
}


Parameters WithStopCriteriaAnalysis::getParameters(const bool withCoefOfVar) const
{
  Parameters param;

  if (withCoefOfVar)
    param.add("Maximum coefficient of variation", getMaximumCoefficientOfVariation());

  String time = "- (s)";
  if ((int)getMaximumElapsedTime() < std::numeric_limits<int>::max() /*for old study*/ &&
      getMaximumElapsedTime() > 0)
    time = (OSS() << getMaximumElapsedTime()).str() + "(s)";
  param.add("Maximum elapsed time", time);

  String maxCalls = "-";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    maxCalls = (OSS() << getMaximumCalls()).str();
  param.add("Maximum calls", maxCalls);

  return param;
}


/* String converter */
String WithStopCriteriaAnalysis::__repr__() const
{
  OSS oss;
  oss << " maximumCalls=";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getMaximumCalls();
  else
    oss << "inf";
  oss << " maximumCoefficientOfVariation=" << getMaximumCoefficientOfVariation()
      << " maximumElapsedTime=";
  if (getMaximumElapsedTime() < std::numeric_limits<double>::max() && getMaximumElapsedTime() > 0)
    oss << getMaximumElapsedTime();
  else
    oss << "inf";

  return oss;
}


/* Method save() stores the object through the StorageManager */
void WithStopCriteriaAnalysis::save(Advocate & adv) const
{
  adv.saveAttribute("maximumCalls_", maximumCalls_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("maximumElapsedTime_", maximumElapsedTime_);
}


/* Method load() reloads the object from the StorageManager */
void WithStopCriteriaAnalysis::load(Advocate & adv)
{
  adv.loadAttribute("maximumCalls_", maximumCalls_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("maximumElapsedTime_", maximumElapsedTime_);
}
}
