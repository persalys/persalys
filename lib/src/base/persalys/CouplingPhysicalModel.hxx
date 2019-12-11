//                                               -*- C++ -*-
/**
 *  @brief File-exchange coupling model
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
#ifndef PERSALYS_COUPLINGPHYSICALMODEL_HXX
#define PERSALYS_COUPLINGPHYSICALMODEL_HXX

#include "persalys/PythonPhysicalModel.hxx"
#include "persalys/CouplingStep.hxx"

namespace PERSALYS
{
typedef OT::Collection<CouplingStep> CouplingStepCollection;

class PERSALYS_API CouplingPhysicalModel : public PythonPhysicalModel
{
  CLASSNAME

public:

  /* Default constructor */
  explicit CouplingPhysicalModel(const OT::String & name = "Unnamed",
                                 const CouplingStepCollection & steps = CouplingStepCollection());

  /** Virtual constructor */
  virtual CouplingPhysicalModel * clone() const;

  /** Accessor to the steps */
  void setSteps(const CouplingStepCollection & steps);
  CouplingStepCollection getSteps() const;

  OT::Function getFunction() const;

  virtual OT::String getHtmlDescription(const bool deterministic) const;
  OT::String getPythonScript() const;

  OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::String getStepsMacro(const OT::String & offset = "") const;
private:
  // list of steps
  OT::PersistentCollection<CouplingStep> steps_;

};
}
#endif
