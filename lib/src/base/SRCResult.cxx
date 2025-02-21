//                                               -*- C++ -*-
/**
 *  @brief SRCResult contains the Standard Regression Coefficients
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
#include "persalys/SRCResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SRCResult)

static Factory<SRCResult> Factory_SRCResult;

/* Default constructor */
SRCResult::SRCResult()
  : EvaluationResult()
  , outputNames_()
  , indices_()
  , signedIndices_()
  , indicesInterval_()
  , callsNumber_(0)
{
}


/* Virtual constructor */
SRCResult* SRCResult::clone() const
{
  return new SRCResult(*this);
}


Description SRCResult::getOutputNames() const
{
  return outputNames_;
}


Description SRCResult::getInputNames() const
{
  return indices_.getDescription();
}


Sample SRCResult::getIndices() const
{
  return indices_;
}


Sample SRCResult::getSignedIndices() const
{
  return signedIndices_;
}


Collection<Interval> SRCResult::getIndicesInterval() const
{
  return indicesInterval_;
}


Collection<Interval> SRCResult::getSignedIndicesInterval() const
{
  return signedIndicesInterval_;
}


UnsignedInteger SRCResult::getCallsNumber() const
{
  return callsNumber_;
}


Point SRCResult::getR2() const
{
  return r2_;
}


/* String converter */
String SRCResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " outputNames=" << getOutputNames()
      << " indices=" << getIndices()
      << " indices intervals=" << getIndicesInterval()
      << " signed indices=" << getSignedIndices()
      << " signed indices intervals=" << getSignedIndicesInterval()
      << " r2=" << getR2()
      << " calls number=" << getCallsNumber();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SRCResult::save(Advocate & adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("indices_", indices_);
  adv.saveAttribute("signedIndices_", signedIndices_);
  adv.saveAttribute("indicesInterval_", indicesInterval_);
  adv.saveAttribute("signedIndicesInterval_", signedIndicesInterval_);
  adv.saveAttribute("r2_", r2_);
  adv.saveAttribute("callsNumber_", callsNumber_);
}


/* Method load() reloads the object from the StorageManager */
void SRCResult::load(Advocate & adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("indices_", indices_);
  adv.loadAttribute("signedIndices_", signedIndices_);
  adv.loadAttribute("indicesInterval_", indicesInterval_);
  adv.loadAttribute("signedIndicesInterval_", signedIndicesInterval_);
  adv.loadAttribute("r2_", r2_);
  adv.loadAttribute("callsNumber_", callsNumber_);
}
}
