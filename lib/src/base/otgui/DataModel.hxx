//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class to define data model
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
#ifndef OTGUI_DATAMODEL_HXX
#define OTGUI_DATAMODEL_HXX

#include "DataModelImplementation.hxx"

namespace OTGUI {
class OTGUI_API DataModel : public OT::TypedInterfaceObject<DataModelImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<DataModelImplementation> Implementation;
  typedef OT::Collection<OT::NumericalSample> NumericalSampleCollection;

  /** Default constructor */
  DataModel();
  /** Default constructor */
  DataModel(const DataModelImplementation & implementation);
  /** Constructor from implementation */
  DataModel(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  DataModel(DataModelImplementation * p_implementation);

  /** Constructor with parameters */
  DataModel(const OT::String & name);
  /** Constructor with parameters */
  DataModel(const OT::String & name,
            const OT::String & fileName,
            const OT::Indices & inputColumns,
            const OT::Indices & outputColumns=OT::Indices(),
            const OT::Description & inputNames=OT::Description(),
            const OT::Description & outputNames=OT::Description());
  /** Constructor with parameters */
  DataModel(const OT::String & name,
            const OT::NumericalSample & inSample,
            const OT::NumericalSample & outSample);

  void addObserver(Observer * observer);

  OT::NumericalSample getInputSample() const;
  void setInputSample(const OT::NumericalSample & sample);

  OT::NumericalSample getOutputSample() const;
  void setOutputSample(const OT::NumericalSample & sample);

  NumericalSampleCollection getListXMin() const;
  NumericalSampleCollection getListXMax() const;

  OT::NumericalSample getSample() const;

  OT::String getFileName() const;
  void setFileName(const OT::String & fileName);

  OT::Indices getInputColumns() const;
  void setInputColumns(const OT::Indices & inputColumns);
  OT::Indices getOutputColumns() const;
  void setColumns(const OT::Indices & inputColumns,
                  const OT::Indices & outputColumns,
                  const OT::Description & inputNames=OT::Description(),
                  const OT::Description & outputNames=OT::Description());

  OT::Description getInputNames() const;
  OT::Description getOutputNames() const;

  virtual OT::String getPythonScript() const;
};
}
#endif