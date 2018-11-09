//                                               -*- C++ -*-
/**
 *  @brief SobolResult contains the Sobol indices
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
#include "otgui/SobolResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(SobolResult)

static Factory<SobolResult> Factory_SobolResult;
static Factory<PersistentCollection<Interval> > Factory_IntervalColl;

/* Default constructor */
SobolResult::SobolResult()
  : PersistentObject()
  , outputNames_()
  , firstOrderIndices_()
  , totalIndices_()
  , firstOrderIndicesInterval_()
  , totalIndicesInterval_()
  , callsNumber_(0)
  , elapsedTime_(0.)
  , confidenceIntervalLength_(-1.)
{
}


/* Constructor with parameters */
SobolResult::SobolResult(const Sample& firstOrderIndices,
                         const Sample& totalIndices,
                         const Description& outputNames)
  : PersistentObject()
  , outputNames_(outputNames)
  , firstOrderIndices_(firstOrderIndices)
  , totalIndices_(totalIndices)
  , firstOrderIndicesInterval_()
  , totalIndicesInterval_()
  , callsNumber_(0)
  , elapsedTime_(0.)
  , confidenceIntervalLength_(-1.)
{
  if (!(firstOrderIndices.getDimension() && totalIndices.getDimension() && outputNames.getSize()))
    throw InvalidArgumentException(HERE) << "SobolResult: All the arguments must have the same non-zero dimension";
  if (firstOrderIndices.getDimension() != totalIndices.getDimension())
    throw InvalidArgumentException(HERE) << "SobolResult: The list of first order indices and the list of total indices must have the same dimension";
  if (firstOrderIndices.getSize() != totalIndices.getSize())
    throw InvalidArgumentException(HERE) << "SobolResult: The list of first order indices and the list of total indices must have the same size";
  if (firstOrderIndices.getSize() != outputNames.getSize())
    throw InvalidArgumentException(HERE) << "SobolResult: The list of output names and the list of first order indices must have the same size";
}


/* Virtual constructor */
SobolResult* SobolResult::clone() const
{
  return new SobolResult(*this);
}


Description SobolResult::getOutputNames() const
{
  return outputNames_;
}


Description SobolResult::getInputNames() const
{
  return firstOrderIndices_.getDescription();
}


Sample SobolResult::getFirstOrderIndices() const
{
  return firstOrderIndices_;
}


Sample SobolResult::getTotalIndices() const
{
  return totalIndices_;
}


Collection<Interval> SobolResult::getFirstOrderIndicesInterval() const
{
  return firstOrderIndicesInterval_;
}


Collection<Interval> SobolResult::getTotalIndicesInterval() const
{
  return totalIndicesInterval_;
}


UnsignedInteger SobolResult::getCallsNumber() const
{
  return callsNumber_;
}


Scalar SobolResult::getElapsedTime() const
{
  return elapsedTime_;
}


Scalar SobolResult::getConfidenceIntervalLength() const
{
  return confidenceIntervalLength_;
}


/* String converter */
String SobolResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " outputNames=" << getOutputNames()
      << " firstOrderIndices=" << getFirstOrderIndices()
      << " totalIndices=" << getTotalIndices()
      << " firstOrderIndicesInterval=" << getFirstOrderIndicesInterval()
      << " totalIndicesInterval=" << getTotalIndicesInterval()
      << " callsNumber=" << getCallsNumber()
      << " confidenceIntervalLength=" << getConfidenceIntervalLength();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SobolResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("firstOrderIndices_", firstOrderIndices_);
  adv.saveAttribute("totalIndices_", totalIndices_);
  adv.saveAttribute("firstOrderIndicesInterval_", firstOrderIndicesInterval_);
  adv.saveAttribute("totalIndicesInterval_", totalIndicesInterval_);
  adv.saveAttribute("elapsedTime_", elapsedTime_);
  adv.saveAttribute("callsNumber_", callsNumber_);
  adv.saveAttribute("confidenceIntervalLength_", confidenceIntervalLength_);
}


/* Method load() reloads the object from the StorageManager */
void SobolResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("firstOrderIndices_", firstOrderIndices_);
  adv.loadAttribute("totalIndices_", totalIndices_);
  adv.loadAttribute("firstOrderIndicesInterval_", firstOrderIndicesInterval_);
  adv.loadAttribute("totalIndicesInterval_", totalIndicesInterval_);
  adv.loadAttribute("elapsedTime_", elapsedTime_);
  adv.loadAttribute("callsNumber_", callsNumber_);
  adv.loadAttribute("confidenceIntervalLength_", confidenceIntervalLength_);
}
}
