//                                               -*- C++ -*-
/**
 *  @brief Class for analysis with stop criteria
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
#include "otgui/WithStopCriteriaAnalysis.hxx"

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
WithStopCriteriaAnalysis::WithStopCriteriaAnalysis()
  : maximumCalls_(std::numeric_limits<int>::max())
  , maximumCoefficientOfVariation_(0.01)
  , maximumElapsedTime_(60) // in seconds
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
{
}


WithStopCriteriaAnalysis::~WithStopCriteriaAnalysis()
{
}


bool WithStopCriteriaAnalysis::Stop(void * p)
{
  TimeCriteria * arg = (TimeCriteria*)p;
  arg->elapsedTime_ = clock()-arg->startTime_;
  // stop algorithm when the elapsed time is superior to the max elapsed time
  if (arg->stopRequested_ || (arg->elapsedTime_ > arg->maxElapsedTime_))
    return true;
  return false;
}


UnsignedInteger WithStopCriteriaAnalysis::getMaximumCalls() const
{
  return maximumCalls_;
}


void WithStopCriteriaAnalysis::setMaximumCalls(const UnsignedInteger maxi)
{
  if (maxi < 1)
    throw InvalidValueException(HERE) << "The maximum calls number can not be null";
  if (maxi < blockSize_)
    throw InvalidValueException(HERE) << "The maximum calls number can not be inferior to the block size " << blockSize_;
  maximumCalls_ = maxi;
}


double WithStopCriteriaAnalysis::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}


void WithStopCriteriaAnalysis::setMaximumCoefficientOfVariation(const double coef)
{
  maximumCoefficientOfVariation_ = coef;
}


UnsignedInteger WithStopCriteriaAnalysis::getMaximumElapsedTime() const
{
  return maximumElapsedTime_;
}


void WithStopCriteriaAnalysis::setMaximumElapsedTime(const UnsignedInteger seconds)
{
  if (seconds < 1)
    throw InvalidValueException(HERE) << "The maximum elapsed time must be superior to 0 second";
  maximumElapsedTime_ = seconds;
}


UnsignedInteger WithStopCriteriaAnalysis::getBlockSize() const
{
  return blockSize_;
}


void WithStopCriteriaAnalysis::setBlockSize(const UnsignedInteger size)
{
  if (size < 1)
    throw InvalidValueException(HERE) << "The block size must be superior to 0";
  if (size > maximumCalls_)
    throw InvalidValueException(HERE) << "The block size can not be superior to the maximum calls " << maximumCalls_;
  blockSize_ = size;
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
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getMaximumElapsedTime();
  else
    oss << "inf";
  oss << " blockSize=" << getBlockSize();

  return oss;
}


/* Method save() stores the object through the StorageManager */
void WithStopCriteriaAnalysis::save(Advocate & adv) const
{
  adv.saveAttribute("maximumCalls_", maximumCalls_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("maximumElapsedTime_", maximumElapsedTime_);
  adv.saveAttribute("blockSize_", blockSize_);
}


/* Method load() reloads the object from the StorageManager */
void WithStopCriteriaAnalysis::load(Advocate & adv)
{
  adv.loadAttribute("maximumCalls_", maximumCalls_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("maximumElapsedTime_", maximumElapsedTime_);
  adv.loadAttribute("blockSize_", blockSize_);
}
}