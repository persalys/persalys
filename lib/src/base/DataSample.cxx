//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/DataSample.hxx"

#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
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

  for (UnsignedInteger j = 0; j < getOutputSample().getDimension(); ++j)
  {
    Sample inMinSample(0, getInputSample().getDimension());
    Sample inMaxSample(0, getInputSample().getDimension());

    Pointer<SampleImplementation> sampleOut = getOutputSample().getMarginal(j).getImplementation();
    std::pair<OT::NSI_iterator, OT::NSI_iterator> minmax = std::minmax_element(sampleOut->begin(),
                                                                               sampleOut->end());

    // first occurrence of min idx
    const UnsignedInteger minIdx = minmax.first - sampleOut->begin();
    // last occurrence of max idx
    const UnsignedInteger maxIdx = minmax.second - sampleOut->begin();

    inMinSample.add(getInputSample()[minIdx]);
    inMaxSample.add(getInputSample()[maxIdx]);

    // look for antoher occurrence of min
    if (minmax.first != sampleOut->end())
    {
      for (OT::NSI_iterator it = std::next(minmax.first); it != sampleOut->end(); ++it)
      {
        if (*it == *(minmax.first)) {
          inMinSample.add(getInputSample()[it - sampleOut->begin()]);
          break;
        }
      }
    }

    // look for antoher occurrence of max
    if (minmax.second != sampleOut->begin())
    {
      for (OT::NSI_iterator it = std::prev(minmax.second); it != sampleOut->begin(); --it)
      {
        if (*it == *(minmax.second))
        {
          inMaxSample.add(getInputSample()[it - sampleOut->begin()]);
          break;
        }
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
