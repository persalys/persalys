//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel validation result
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
#include "persalys/MetaModelValidationResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(MetaModelValidationResult)

static Factory<MetaModelValidationResult> Factory_MetaModelValidationResult;

MetaModelValidationResult::MetaModelValidationResult()
  : PersistentObject()
  , name_("")
{
}


MetaModelValidationResult::MetaModelValidationResult(const String& name)
  : PersistentObject()
  , name_(name)
{
}



MetaModelValidationResult::MetaModelValidationResult(const Sample& metaModelSample,
    const Point& q2,
    const Point& residuals)
  : PersistentObject()
  , name_("")
  , metaModelSample_(metaModelSample)
  , q2_(q2)
  , residuals_(residuals)
{
}


MetaModelValidationResult * MetaModelValidationResult::clone() const
{
  return new MetaModelValidationResult(*this);
}


void MetaModelValidationResult::add(const MetaModelValidationResult& other)
{
  if ((other.getMetaModelOutputSample().getDimension() * other.q2_.getDimension() * other.residuals_.getDimension()) != 1)
    throw InvalidArgumentException(HERE) << "Method misused: the result must contain only one result";

  if (!metaModelSample_.getSize())
  {
    name_ = other.getName();
    metaModelSample_ = other.metaModelSample_;
    parameters_ = other.getParameters();
  }
  else
  {
    if (metaModelSample_.getSize() != other.metaModelSample_.getSize())
      throw InvalidArgumentException(HERE) << "Method misused: the metamodelSamples must have the same dimension";
    metaModelSample_.stack(other.metaModelSample_);
  }
  q2_.add(other.q2_[0]);
  residuals_.add(other.residuals_[0]);
}


String MetaModelValidationResult::getName() const
{
  return name_;
}


PointWithDescription MetaModelValidationResult::getParameters() const
{
  return parameters_;
}


Sample MetaModelValidationResult::getMetaModelOutputSample() const
{
  return metaModelSample_;
}


Point MetaModelValidationResult::getResiduals() const
{
  return residuals_;
}


Point MetaModelValidationResult::getQ2() const
{
  return q2_;
}


/* String converter */
String MetaModelValidationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " parameters=" << getParameters()
      << " residuals=" << getResiduals()
      << " q2=" << getQ2();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MetaModelValidationResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("name_", name_);
  adv.saveAttribute("parameters_", parameters_);
  adv.saveAttribute("metaModelSample_", metaModelSample_);
  adv.saveAttribute("q2_", q2_);
  adv.saveAttribute("residuals_", residuals_);
}


/* Method load() reloads the object from the StorageManager */
void MetaModelValidationResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("name_", name_);
  adv.loadAttribute("parameters_", parameters_);
  adv.loadAttribute("metaModelSample_", metaModelSample_);
  adv.loadAttribute("q2_", q2_);
  adv.loadAttribute("residuals_", residuals_);
}
}
