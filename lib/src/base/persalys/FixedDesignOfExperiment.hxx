//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_FIXEDDESIGNOFEXPERIMENT_HXX
#define PERSALYS_FIXEDDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API FixedDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  /** Default constructor */
  FixedDesignOfExperiment();
  /** Constructor with parameters */
  FixedDesignOfExperiment(const OT::String& name, const PhysicalModel& physicalModel);
  /** Constructor with parameters */
  FixedDesignOfExperiment(const OT::String& name,
                          const PhysicalModel& physicalModel,
                          const OT::Sample& inputSample);

  /** Virtual constructor */
  FixedDesignOfExperiment * clone() const override;

  void setOriginalInputSample(const OT::Sample & sample);

  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

protected:
  OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const override;
};
}
#endif
