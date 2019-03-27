//                                               -*- C++ -*-
/**
 *  @brief Retrieve information from an FMU file
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
#ifndef OTGUI_FMUINFO_HXX
#define OTGUI_FMUINFO_HXX

#include <openturns/Indices.hxx>
#include <openturns/Description.hxx>
#include <openturns/Point.hxx>

#include "otgui/OTGuiprivate.hxx"

namespace OTGUI
{

class OTGUI_API FMUInfo : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  FMUInfo();

  /** Constructor with parameters */
  explicit FMUInfo(const OT::String & fileName);

  virtual FMUInfo * clone() const;

  /** String converter */
  virtual OT::String __repr__() const;

  OT::String getFileName() const;

  // properties accessors
  OT::String getAuthor() const;
  OT::String getCopyright() const;
  OT::String getTool() const;
  OT::String getDateTime() const;
  OT::String getGUID() const;
  OT::String getIdentifier() const;
  OT::String getPlatform() const;
  OT::String getVersion() const;
  OT::String getFMIVersion() const;
  OT::String getModelType() const;

  // variables accessors
  OT::Description getVariableNames() const;
  OT::Indices getCausality() const;
  OT::Indices getVariability() const;
  OT::Description getStartKeys() const;
  OT::Point getStartValues() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(OT::Advocate & adv);

protected:
  void initialize();

private:

  OT::String fileName_;

  // properties of the model
  OT::String author_;
  OT::String copyright_;
  OT::String tool_;
  OT::String dateTime_;
  OT::String guid_;
  OT::String identifier_;
  OT::String platform_;
  OT::String version_;
  OT::String fmiVersion_;
  OT::String modelType_;

  // variables
  OT::Description variableNames_;
  OT::Indices causality_;
  OT::Indices variability_;
  OT::Description startKeys_;
  OT::Point startValues_;
};
}
#endif
