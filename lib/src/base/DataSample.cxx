//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/DataSample.hxx"

#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace OTGUI
{

/* Default constructor */
DataSample::DataSample()
  : PersistentObject()
  , inputSample_()
  , outputSample_()
  , sample_()
  , listXMin_()
  , listXMax_()
{
}


/* Constructor with parameters */
DataSample::DataSample(const Sample & inSample, const Sample & outSample)
  : PersistentObject()
  , inputSample_(inSample)
  , outputSample_(outSample)
  , sample_()
  , listXMin_()
  , listXMax_()
{
  if (getInputSample().getSize() && getOutputSample().getSize())
    if (getInputSample().getSize() != getOutputSample().getSize())
      throw InvalidDimensionException(HERE) << "The input sample and the output sample must have the same size";
}


/* Virtual constructor */
DataSample* DataSample::clone() const
{
  return new DataSample(*this);
}


Sample DataSample::getInputSample() const
{
  return inputSample_;
}


void DataSample::setInputSample(const Sample & sample)
{
  inputSample_ = sample;
  sample_ = Sample();
  listXMin_.clear();
  listXMax_.clear();
}


Sample DataSample::getOutputSample() const
{
  return outputSample_;
}


void DataSample::setOutputSample(const Sample & sample)
{
  outputSample_ = sample;
  sample_ = Sample();
  listXMin_.clear();
  listXMax_.clear();
}


DataSample::SampleCollection DataSample::getListXMin() const
{
  if (!listXMin_.getSize())
    searchMinMax();
  return listXMin_;
}


DataSample::SampleCollection DataSample::getListXMax() const
{
  if (!listXMax_.getSize())
    searchMinMax();
  return listXMax_;
}


void DataSample::searchMinMax() const
{
  if (getInputSample().getSize() != getOutputSample().getSize())
    throw InvalidDimensionException(HERE) << "The input sample and the output sample must have the same size";

  // get the minimum and the maximum output values
  const Point outputMin = getOutputSample().getMin();
  const Point outputMax = getOutputSample().getMax();

  // find for each output, the input points where the output is max and min
  // (we can have several input points for each extremum)
  for (UnsignedInteger out = 0; out < getOutputSample().getDimension(); ++out)
  {
    Sample inMinSample(0, getInputSample().getDimension());
    Sample inMaxSample(0, getInputSample().getDimension());

    // if output min = output max : take all the input points
    if (outputMin[out] == outputMax[out])
    {
      inMinSample = getInputSample();
      inMaxSample = getInputSample();
    }
    // else : find the input values for the output min and max
    else
    {
      for (UnsignedInteger i = 0; i < getInputSample().getSize(); ++i)
      {
        if (getOutputSample()(i, out) == outputMin[out])
          inMinSample.add(getInputSample()[i]);
        if (getOutputSample()(i, out) == outputMax[out])
          inMaxSample.add(getInputSample()[i]);
      }
    }
    listXMin_.add(inMinSample);
    listXMax_.add(inMaxSample);
  }
}


Sample DataSample::getSample() const
{
  if (!sample_.getSize() && (getInputSample().getSize() || getOutputSample().getSize()))
  {
    if (getInputSample().getSize() == getOutputSample().getSize())
    {
      sample_ = getInputSample();
      sample_.stack(getOutputSample());
    }
    else
    {
      if (getInputSample().getSize() && !getOutputSample().getSize())
        sample_ = getInputSample();
      else if (!getInputSample().getSize() && getOutputSample().getSize())
        sample_ = getOutputSample();
      else // input sample and output sample have different sizes
        throw InvalidDimensionException(HERE) << "The input sample and the output sample must have the same size";
    }
  }
  return sample_;
}


bool DataSample::isValid() const
{
  for (UnsignedInteger i = 0; i < getSample().getSize(); ++i)
  {
    for (UnsignedInteger j = 0; j < sample_.getDimension(); ++j)
    {
      if (!SpecFunc::IsNormal(sample_(i, j)))
      {
        return false;
      }
    }
  }
  return true;
}


/* Method save() stores the object through the StorageManager */
void DataSample::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("listXMin_", listXMin_);
  adv.saveAttribute("listXMax_", listXMax_);
}


/* Method load() reloads the object from the StorageManager */
void DataSample::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("listXMin_", listXMin_);
  adv.loadAttribute("listXMax_", listXMax_);
}
}
