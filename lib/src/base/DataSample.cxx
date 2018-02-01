//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/DataSample.hxx"

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

  const UnsignedInteger size = getInputSample().getSize();
  const UnsignedInteger numberInputs = getInputSample().getDimension();

  Indices indicesInputs(numberInputs);
  indicesInputs.fill();

  Sample sample(getInputSample());
  sample.stack(getOutputSample());

  for (UnsignedInteger i = numberInputs; i < sample.getDimension(); ++i)
  {
    Sample orderedSample(sample.sortAccordingToAComponent(i));

    // Search min value of the ith output and the corresponding set of inputs X
    const double minValue = orderedSample(0, i);

    UnsignedInteger it = 0;
    double value = orderedSample(it, i);
    Sample tempSample(0, numberInputs);
    do
    {
      const Point point(orderedSample.getMarginal(indicesInputs)[it]);
      if (!tempSample.contains(point))
        tempSample.add(point);
      ++it;
      value = orderedSample(it, i);
    }
    while (value == minValue && it < size);

    listXMin_.add(tempSample);

    // Search max value of the ith output and the corresponding set of inputs X
    const double maxValue = orderedSample(size - 1, i);

    it = 0;
    value = orderedSample(size - 1 - it, i);
    tempSample = Sample(0, numberInputs);
    do
    {
      const Point point(orderedSample.getMarginal(indicesInputs)[size - 1 - it]);
      if (!tempSample.contains(point))
        tempSample.add(point);
      ++it;
      value = orderedSample(size - 1 - it, i);
    }
    while (value == maxValue && it < size);

    listXMax_.add(tempSample);
  }
}


Sample DataSample::getSample() const
{
  if (!sample_.getSize())
  {
    Sample sample;

    if (getInputSample().getSize() == getOutputSample().getSize())
    {
      sample = getInputSample();
      sample.stack(getOutputSample());
    }
    else
    {
      if (getInputSample().getSize() && !getOutputSample().getSize())
        sample = getInputSample();
      else if (!getInputSample().getSize() && getOutputSample().getSize())
        sample = getOutputSample();
      else // input sample and output sample have different sizes
        throw InvalidDimensionException(HERE) << "The input sample and the output sample must have the same size";
    }
    sample_ = sample;
  }
  return sample_;
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
