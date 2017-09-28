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
#ifndef OTGUI_IMPORTEDDESIGNOFEXPERIMENT_HXX
#define OTGUI_IMPORTEDDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"

namespace OTGUI {
class OTGUI_API ImportedDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  /** Default constructor */
  ImportedDesignOfExperiment();
  /** Constructor with parameters */
  ImportedDesignOfExperiment(const OT::String& name, const PhysicalModel& physicalModel);
  /** Constructor with parameters */
  ImportedDesignOfExperiment(const OT::String& name,
                             const PhysicalModel& physicalModel,
                             const OT::String& fileName,
                             const OT::Indices& inputColumns);

  /** Virtual constructor */
  virtual ImportedDesignOfExperiment * clone() const;

  OT::String getFileName() const;
  void setFileName(const OT::String& fileName);
  OT::Indices getInputColumns() const;
  void setInputColumns(const OT::Indices& inputColumns);

  OT::Sample getSampleFromFile() const;
  static OT::Sample ImportSample(const OT::String& fileName);

  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv);

protected:
  virtual OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const;

protected:
  OT::String fileName_;
  OT::Indices inputColumns_;
  mutable OT::Sample sampleFromFile_;
};
}
#endif
