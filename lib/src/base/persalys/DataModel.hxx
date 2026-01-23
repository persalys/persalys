//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
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
#ifndef PERSALYS_DATAMODEL_HXX
#define PERSALYS_DATAMODEL_HXX

#include "DesignOfExperiment.hxx"
#include "ImportedDataset.hxx"

#include <optional>

namespace PERSALYS
{
class PERSALYS_BASE_API DataModel : public DesignOfExperimentImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  explicit DataModel(const OT::String & name = "Unnamed");

  /** Constructor with parameters */
  DataModel(const OT::String & name,
            const OT::String & fileName,
            const OT::Indices & inputColumns,
            const OT::Indices & outputColumns = OT::Indices(),
            const OT::Description & inputNames = OT::Description(),
            const OT::Description & outputNames = OT::Description());

  /** Constructor with parameters */
  DataModel(const OT::String & name,
            const OT::Sample & inSample,
            const OT::Sample & outSample);

  DataModel(const OT::String & name,
            const PhysicalModel & physicalModel,
            const std::optional<ImportedDataset> & importedDataset = std::nullopt,
            const OT::Description & inputNames = OT::Description(),
            const OT::Description & outputNames = OT::Description());

  /** Virtual constructor */
  DataModel * clone() const override;

  void removeAllObservers() override;

  void setInputSample(const OT::Sample & sample) override;
  void setOutputSample(const OT::Sample & sample) override;

  virtual OT::Description getInputNames() const;
  virtual OT::Description getOutputNames() const;
  virtual void setColumns(const OT::Indices &inputColumns,
                          const OT::Description &inputNames,
                          const OT::Indices &outputColumns,
                          const OT::Description &outputNames);

  void setSample(const OT::Sample & sample);

  OT::Sample getSampleFromFile() const;
  
  OT::Indices getInputColumns() const;
  OT::Indices getOutputColumns() const;

  OT::String getFileName() const;
  void setFileName(const OT::String & fileName);

  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;


protected:
  void setNames(const OT::Description &inputNames, const OT::Description &outputNames);
  virtual void update();

protected:
  std::optional<ImportedDataset> importedDataset_ = std::nullopt;
};
}
#endif
