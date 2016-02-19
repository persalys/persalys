//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_YACSPHYSICALMODEL_HXX
#define OTGUI_YACSPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"
#include "YACSEvaluation.hxx"

namespace OTGUI {
class YACSPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  /* Default constructor */
  YACSPhysicalModel(const OT::String & name="Unnamed");
  /* Constructor with parameters */
  YACSPhysicalModel(const OT::String & name, const OT::String & fileName);

  /** Virtual constructor */
  virtual YACSPhysicalModel * clone() const;

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);

  void setOutputs(const OutputCollection & outputs);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);

  OT::String getXMLFileName() const;
  void setXMLFileName(const OT::String & fileName);

  OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction();

  OT::String dump() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void updateData();

private:
  YACSEvaluation evaluation_;
  OT::String xmlFileName_;
};
}
#endif