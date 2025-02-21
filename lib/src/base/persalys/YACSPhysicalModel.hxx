//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
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
#ifndef PERSALYS_YACSPHYSICALMODEL_HXX
#define PERSALYS_YACSPHYSICALMODEL_HXX

#include "persalys/PhysicalModelImplementation.hxx"
#include "persalys/YACSEvaluation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API YACSPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME

public:
  /* Default constructor */
  YACSPhysicalModel(const OT::String & name = "Unnamed");

  /* Constructor with parameters */
  YACSPhysicalModel(const OT::String & name,
                    const InputCollection & inputs,
                    const OutputCollection & outputs,
                    const OT::String & script);

  /** Virtual constructor */
  YACSPhysicalModel * clone() const override;

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);

  void setOutputs(const OutputCollection & outputs);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);

  /** Accessor to the text script of the model (python, xml, whatever). */
  OT::String getCode() const;
  void setCode(const OT::String & code);

  /** Accesor to launching resource properties */
  ydefx::JobParametersProxy& jobParameters();
  const ydefx::JobParametersProxy& jobParameters() const;

  /** The job model is a python object which drives the execution of the job.*/
  void setJobModel(const py2cpp::PyPtr& model);

  OT::String getHtmlDescription(const bool deterministic) const override;
  OT::String getPythonScript() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

  /** String converter */
  OT::String __repr__() const override;

  virtual void acceptLaunchParameters(LaunchParametersVisitor* visitor);

  static void ReplaceInString(OT::String & workString, const OT::String & strToReplace, const OT::String & newValue);

protected:
  void updateData();

  OT::Function generateFunction(const OT::Description & outputNames) const override;
  virtual OT::String getJobParamsPythonScript() const;

private:
  mutable YACSEvaluation evaluation_;
  mutable OT::Function functionCache_;
};
}
#endif
