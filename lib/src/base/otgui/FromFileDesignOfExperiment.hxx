//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef OTGUI_FROMFILEDESIGNOFEXPERIMENT_HXX
#define OTGUI_FROMFILEDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentImplementation.hxx"

namespace OTGUI {
class OTGUI_API FromFileDesignOfExperiment : public DesignOfExperimentImplementation
{
  CLASSNAME;

public:
  /** Default constructor */
  FromFileDesignOfExperiment();
  /** Constructor with parameters */
  FromFileDesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel);
  /** Constructor with parameters */
  FromFileDesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel,
                             const OT::String & fileName, const OT::Indices & inputColumns);

  /** Virtual constructor */
  virtual FromFileDesignOfExperiment * clone() const;

  OT::String getFileName() const;
  virtual void setFileName(const OT::String & fileName);
  OT::Indices getInputColumns() const;
  virtual void setInputColumns(const OT::Indices & inputColumns);

  static OT::NumericalSample ImportSample(const OT::String & fileName);

  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void generateInputSample();

  OT::String fileName_;
  OT::Indices inputColumns_;
  OT::NumericalSample sampleFromFile_;
};
}
#endif