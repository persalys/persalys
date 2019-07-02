//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
class PERSALYS_API DataModel : public DesignOfExperimentImplementation, public DataImport
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
  virtual DataModel * clone() const;

  OT::Description getInputNames();
  OT::Description getOutputNames();
  void setNames(const OT::Description & inputNames, const OT::Description & outputNames);

  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void check();
  virtual void update();

private:
  OT::Description inputNames_;
  OT::Description outputNames_;
};
}
#endif
