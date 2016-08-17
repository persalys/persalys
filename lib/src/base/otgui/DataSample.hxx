//                                               -*- C++ -*-
/**
 *  @brief Class to define data sample
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
#ifndef OTGUI_DATASAMPLE_HXX
#define OTGUI_DATASAMPLE_HXX

#include "otgui/OTGuiprivate.hxx"
#include "openturns/OTType.hxx"

namespace OTGUI {
class OTGUI_API DataSample
{
//   CLASSNAME;

public:
  typedef OT::Collection<OT::NumericalSample> NumericalSampleCollection;

  friend class FixedDesignOfExperiment;
  friend class FromFileDesignOfExperiment;

  /** Default constructor */
  DataSample();
  /** Constructor with parameters */
  DataSample(const OT::NumericalSample & inSample, const OT::NumericalSample & outSample);

  /** Virtual constructor */
  virtual DataSample * clone() const;

  OT::NumericalSample getInputSample() const;
  void setInputSample(const OT::NumericalSample & sample);

  OT::NumericalSample getOutputSample() const;
  void setOutputSample(const OT::NumericalSample & sample);

  NumericalSampleCollection getListXMin() const;
  NumericalSampleCollection getListXMax() const;

  OT::NumericalSample getSample() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  void searchMinMax() const;

private:
  OT::NumericalSample inputSample_;
  OT::NumericalSample outputSample_;
  mutable OT::NumericalSample sample_;
  mutable OT::PersistentCollection<OT::NumericalSample> listXMin_;
  mutable OT::PersistentCollection<OT::NumericalSample> listXMax_;
};
}
#endif