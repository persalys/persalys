//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "ImportedDataset.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API ImportedDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  using Type = DesignOfExperiment::Type;

  /** Default constructor */
  ImportedDesignOfExperiment();
  /** Constructor with parameters */
  ImportedDesignOfExperiment(const OT::String& name, const PhysicalModel& physicalModel);
  /** Constructor with parameters */
  ImportedDesignOfExperiment( const OT::String& name,
                              const PhysicalModel& physicalModel,
                              const OT::String& fileName,
                              const OT::Indices& inputColumns,
                              const OT::Indices& outputColumns = OT::Indices(),
                              Type type = Type::UK);

  /** Virtual constructor */
  ImportedDesignOfExperiment * clone() const override;

  void setColumns(const OT::Indices &inputColumns,
                  const OT::Indices &outputColumns = OT::Indices());

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;

  void setType(Type type);
  Type getType() const;

  const ImportedDataset& getImportedDataset() const;

  void setEvaluations(OT::Sample&) override;

  void setFileName(const OT::String &fileName);

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv) override;

protected:
  OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const override;
  void launch() override;
  void saveImportedSampleToResult();

private:
  void loadOldFormat(OT::Advocate& adv);

private:
    Type type_ = Type::UK;
    ImportedDataset importedDataset_;
};
}
#endif
