//                                               -*- C++ -*-
/**
 *  @brief Class to define data models
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

#include "PhysicalModel.hxx"
#include "DataSample.hxx"
#include "ImportedDataset.hxx"

#include <optional>

namespace PERSALYS
{
class PERSALYS_BASE_API DataModel : public DataSample, public Observable
{
  CLASSNAME

public:
  enum Type {GENERIC, MC, QMC, LHS, GRID, MORRIS};

  /** Constructor with parameters */
  explicit DataModel(const OT::String & name = "");

  /** Constructor with parameters */
  DataModel(const OT::String & name, const PhysicalModel & physicalModel);

  /** Constructor with parameters */
  DataModel(
    const OT::String & name,
    const ImportedDataset & importedDataset,
    const OT::Description & inputNames = OT::Description(),
    const OT::Description & outputNames = OT::Description());

  /** Constructor with parameters */
  DataModel(
    const OT::String & name,
    const OT::Sample & inSample,
    const OT::Sample & outSample);
  
  /** Virtual constructor */
  DataModel * clone() const override;
  
protected:
  DataModel(
    const OT::String & name,
    const std::optional<PhysicalModel> & physicalModel,
    const std::optional<ImportedDataset> & importedDataset,
    const OT::Description & inputNames,
    const OT::Description & outputNames);
  
public:

  void setName(const OT::String & name);

  void removeAllObservers() override;

  bool hasPhysicalModel() const;
  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const std::optional<PhysicalModel> & physicalModel);

  void initialize();
  void setInputSample(const OT::Sample & sample) override;
  void setOutputSample(const OT::Sample & sample) override;

  OT::Indices getEffectiveInputIndices() const;

  void setType(Type type);
  Type getType() const;

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

  static OT::String TypeToString(Type type);

  virtual OT::String getPythonScript() const;

  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void setNames(const OT::Description &inputNames, const OT::Description &outputNames);
  virtual void update();
  
private:
    void loadDataModelAttributes(OT::Advocate & adv);

protected:
  std::optional<PhysicalModel>    physicalModel_    = std::nullopt;
  std::optional<ImportedDataset>  importedDataset_  = std::nullopt;
  Type type_ = GENERIC;
  bool resetImportedDataset_ = true;
};

}
#endif
