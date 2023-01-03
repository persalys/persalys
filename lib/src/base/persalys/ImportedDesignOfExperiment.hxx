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
#ifndef PERSALYS_IMPORTEDDESIGNOFEXPERIMENT_HXX
#define PERSALYS_IMPORTEDDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"
#include "DataImport.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API ImportedDesignOfExperiment : public DesignOfExperimentEvaluation, public DataImport
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
  ImportedDesignOfExperiment * clone() const override;

  void setInputColumns(const OT::Indices &inputColumns);

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv) override;

protected:
  void check() override;
  OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const override;
  void setDefaultColumns() override;
};
}
#endif
