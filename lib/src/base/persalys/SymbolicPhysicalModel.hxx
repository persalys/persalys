//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
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
#ifndef PERSALYS_SYMBOLICPHYSICALMODEL_HXX
#define PERSALYS_SYMBOLICPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API SymbolicPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  SymbolicPhysicalModel(const OT::String& name = "Unnamed");
  /** Constructor with parameters */
  SymbolicPhysicalModel(const OT::String& name,
                        const InputCollection& inputs,
                        const OutputCollection& outputs,
                        const OT::Description& formulas);

  /** Virtual constructor */
  virtual SymbolicPhysicalModel * clone() const;

  virtual void addOutput(const Output& output);
  virtual void removeOutput(const OT::String& outputName);
  virtual void setOutputs(const OutputCollection& outputs);
  virtual void setOutputName(const OT::String& outputName, const OT::String& newName);

  OT::Description getFormulas() const;
  void setFormulas(const OT::Description& formulas);

  OT::String getFormula(const OT::String& outputName) const;
  void setFormula(const OT::String& outputName, const OT::String& formula);

  virtual OT::String getHtmlDescription(const bool deterministic) const;
  OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(OT::Advocate & adv);

protected:
  virtual OT::Function generateFunction(const OT::Description & outputNames) const;

private:
  std::map<OT::String, OT::String> formulaForEachOutput_;
};
}
#endif
