//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef PERSALYS_PROBABILISTICDESIGNOFEXPERIMENT_HXX
#define PERSALYS_PROBABILISTICDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API ProbabilisticDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  /** Default constructor */
  ProbabilisticDesignOfExperiment();
  /** Constructor with parameters */
  ProbabilisticDesignOfExperiment(const OT::String& name,
                                  const PhysicalModel& physicalModel,
                                  const OT::UnsignedInteger size = OT::ResourceMap::GetAsUnsignedInteger("WeightedExperiment-DefaultSize"),
                                  const OT::String& designName = "LHS",
                                  const OT::String& spaceFilling = "PhiP",
                                  const OT::UnsignedInteger mcLhsSize = 1000);


  /** Virtual constructor */
  ProbabilisticDesignOfExperiment * clone() const override;

  void setSeed(const OT::UnsignedInteger seed) override;

  OT::String getDesignName() const;
  void setDesignName(const OT::String& name);

  OT::String getSpaceFilling() const;
  void setSpaceFilling(const OT::String& name);

  static OT::Description GetDesignNames();
  static OT::Description GetSpaceFillings();

  OT::UnsignedInteger getSize() const;
  void setSize(const OT::UnsignedInteger size);

  OT::UnsignedInteger getMCLHSSize() const;


  Parameters getParameters() const override;
  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const override;

  static OT::Description DesignNames_;
  static OT::Description SpaceFillings_;

private:
  OT::String designName_;
  OT::String spaceFilling_;
  OT::UnsignedInteger size_;
  OT::UnsignedInteger mcLhsSize_;
};
}
#endif
