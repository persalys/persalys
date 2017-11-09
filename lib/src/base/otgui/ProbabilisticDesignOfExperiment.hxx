//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef OTGUI_PROBABILISTICDESIGNOFEXPERIMENT_HXX
#define OTGUI_PROBABILISTICDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"

namespace OTGUI
{
class OTGUI_API ProbabilisticDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  /** Default constructor */
  ProbabilisticDesignOfExperiment();
  /** Constructor with parameters */
  ProbabilisticDesignOfExperiment(const OT::String& name,
                                  const PhysicalModel& physicalModel,
                                  const OT::UnsignedInteger size = OT::ResourceMap::GetAsUnsignedInteger("WeightedExperiment-DefaultSize"),
                                  const OT::String& designName = "LHS");


  /** Virtual constructor */
  virtual ProbabilisticDesignOfExperiment * clone() const;

  virtual void setSeed(const OT::UnsignedInteger seed);

  OT::String getDesignName() const;
  void setDesignName(const OT::String& name);

  static OT::Description GetDesignNames();

  OT::UnsignedInteger getSize() const;
  void setSize(const OT::UnsignedInteger size);

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const;

  static OT::Description DesignNames_;

private:
  OT::String designName_;
  OT::UnsignedInteger size_;
};
}
#endif
