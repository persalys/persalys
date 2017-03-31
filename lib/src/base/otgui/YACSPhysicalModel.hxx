//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
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
#ifndef OTGUI_YACSPHYSICALMODEL_HXX
#define OTGUI_YACSPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"
#include "YACSEvaluation.hxx"

namespace OTGUI {
class OTGUI_API YACSPhysicalModel : public PhysicalModelImplementation
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

  /** Accessor to the xml file name */
  OT::String getXMLFileName() const;
  void setXMLFileName(const OT::String & fileName);

  /** Accessor to the parallelize status */
  bool getParallelizeStatus() const;
  void setParallelizeStatus(const bool & status);

  /** Accessor to the fitting machines */
  OT::Description getFittingMachines() const;

  /** Accessor to the wanted machine */
  OT::String getWantedMachine() const;
  void setWantedMachine(const OT::String & machine);

  OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  /** String converter */
  virtual OT::String __repr__() const;
protected:
  void updateData();

  virtual OT::NumericalMathFunction generateFunction(const OT::Description & outputNames) const;

private:
  YACSEvaluation evaluation_;
  OT::String xmlFileName_;
};
}
#endif