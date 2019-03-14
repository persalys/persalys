//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_YACSPHYSICALMODEL_HXX
#define OTGUI_YACSPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"
#include "YACSEvaluation.hxx"

namespace OTGUI
{
class OTGUI_API YACSPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME

public:
  /* Default constructor */
  YACSPhysicalModel(const OT::String & name = "Unnamed");
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

  /** Accessor to the text script of the model (python, xml, whatever). */
  OT::String getContent() const;
  void setContent(const OT::String & script);

  /** Accesor to launching resource properties */
  ydefx::JobParametersProxy& jobParameters();
  const ydefx::JobParametersProxy& jobParameters()const;

  virtual OT::String getHtmlDescription(const bool deterministic) const;
  OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  /** String converter */
  virtual OT::String __repr__() const;

  virtual void acceptLaunchParameters(LaunchParametersVisitor* visitor);

protected:
  void updateData();

  virtual OT::Function generateFunction(const OT::Description & outputNames) const;
  virtual OT::String getJobParamsPythonScript() const;

private:
  YACSEvaluation evaluation_;
};
}
#endif
