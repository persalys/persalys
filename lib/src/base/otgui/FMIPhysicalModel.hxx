//                                               -*- C++ -*-
/**
 *  @brief FMIPhysicalModel implements models defined by an fmu file
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
#ifndef OTGUI_FMIPHYSICALMODEL_HXX
#define OTGUI_FMIPHYSICALMODEL_HXX

#include "PythonPhysicalModel.hxx"
#include "FMUInfo.hxx"

namespace OTGUI
{
class OTGUI_API FMIPhysicalModel : public PythonPhysicalModel
{
  CLASSNAME

public:

  /* Default constructor */
  FMIPhysicalModel(const OT::String & name = "Unnamed");

  /* Constructor with parameters */
  FMIPhysicalModel(const OT::String & name,
                   const OT::String & fileName);

  /* Constructor with parameters */
  FMIPhysicalModel(const OT::String & name,
                   const InputCollection & inputs,
                   const OutputCollection & outputs,
                   const OT::String & fileName);

  /** Virtual constructor */
  virtual FMIPhysicalModel * clone() const;

  /** Accessor to the xml file name */
  OT::String getFMUFileName() const;
  void setFMUFileName(const OT::String & fileName);

  /** Accessor to infos */
  FMUInfo getFMUInfo() const;

  OT::Function getFunction() const;

  OT::String getPythonScript() const;

  OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  void reassignVariables(const OT::Description & inputNames,
                         const OT::Description & outputNames);

private:
  OT::String fmuFileName_;
  FMUInfo fmuInfo_;

};
}
#endif
