//                                               -*- C++ -*-
/**
 *  @brief PythonPhysicalModel implements models defined by a Python script
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_PYTHONPHYSICALMODEL_HXX
#define PERSALYS_PYTHONPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API PythonPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  PythonPhysicalModel(const OT::String & name = "Unnamed");

  /** Constructor with parameters */
  PythonPhysicalModel(const OT::String & name,
                      const InputCollection & inputs,
                      const OutputCollection & outputs,
                      const OT::String & code);

  /** Virtual constructor */
  virtual PythonPhysicalModel * clone() const;

  virtual void setCode(const OT::String & code);
  OT::String getCode() const;

  virtual OT::String getHtmlDescription(const bool deterministic) const;
  OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);

  void setOutputs(const OutputCollection & outputs);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);

  virtual void setParallel(const OT::Bool flag);
  void resetCallsNumber();

  void setEvalTime(const OT::Scalar& evalTime);
  OT::Scalar getEvalTime() const;

protected:
  virtual OT::Function generateFunction(const OT::Description & outputNames) const;

private:
  OT::String code_;
  mutable OT::Function functionCache_;
};

}
#endif
