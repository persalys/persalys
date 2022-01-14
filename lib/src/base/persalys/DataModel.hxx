//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
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
#ifndef PERSALYS_DATAMODEL_HXX
#define PERSALYS_DATAMODEL_HXX

#include "DesignOfExperiment.hxx"
#include "DataImport.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API DataModel : public DesignOfExperimentImplementation, public DataImport
{
  CLASSNAME

public:
  /** Default constructor */
  DataModel(const OT::String & name = "Unnamed");

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

  /** Constructor with parameters */
  DataModel(const OT::String & name,
            const DesignOfExperiment & doe);

  /** Virtual constructor */
  DataModel * clone() const override;

  void removeAllObservers() override;

  virtual OT::Description getInputNames();
  virtual OT::Description getOutputNames();
  using DataImport::setColumns;
  virtual void setColumns(const OT::Indices &inputColumns,
                          const OT::Description &inputNames,
                          const OT::Indices &outputColumns,
                          const OT::Description &outputNames);

  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

  void setSample(const OT::Sample & sample);

protected:
  void setNames(const OT::Description &inputNames, const OT::Description &outputNames);
  void check() override;
  virtual void update();
  void setDefaultColumns() override;

protected:
  OT::Description inputNames_;
  OT::Description outputNames_;
};
}
#endif
