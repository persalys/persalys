//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
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
#ifndef OTGUI_DATAMODELIMPLEMENTATION_HXX
#define OTGUI_DATAMODELIMPLEMENTATION_HXX

#include "DataSample.hxx"
#include "ImportedSample.hxx"
#include "Observable.hxx"

namespace OTGUI {
class OTGUI_API DataModelImplementation : public OT::PersistentObject, public DataSample,
                                          public ImportedSample, public Observable
{
  CLASSNAME;

public:
  /** Default constructor */
  explicit DataModelImplementation(const OT::String & name="Unnamed");

  /** Constructor with parameters */
  DataModelImplementation(const OT::String & name, const OT::String & fileName,
                          const OT::Indices & inputColumns, const OT::Indices & outputColumns,
                          const OT::Description & inputNames=OT::Description(),
                          const OT::Description & outputNames=OT::Description());

  /** Constructor with parameters */
  DataModelImplementation(const OT::String & name,
                          const OT::NumericalSample & inSample,
                          const OT::NumericalSample & outSample);

  /** Virtual constructor */
  virtual DataModelImplementation * clone() const;

  void setFileName(const OT::String & fileName);

  void setInputColumns(const OT::Indices & inputColumns);
  OT::Indices getOutputColumns() const;
  void setColumns(const OT::Indices & inputColumns, const OT::Indices & outputColumns,
                  const OT::Description & inputNames=OT::Description(),
                  const OT::Description & outputNames=OT::Description());

  OT::Description getInputNames() const;
  OT::Description getOutputNames() const;

  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description inputNames_;
  OT::Description outputNames_;
  OT::Indices outputColumns_;
};
}
#endif