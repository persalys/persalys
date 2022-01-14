//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
  SymbolicPhysicalModel * clone() const override;

  void addOutput(const Output& output) override;
  void removeOutput(const OT::String& outputName) override;
  void setOutputs(const OutputCollection& outputs) override;
  void setOutputName(const OT::String& outputName, const OT::String& newName) override;

  OT::Description getFormulas() const;
  void setFormulas(const OT::Description& formulas);

  OT::String getFormula(const OT::String& outputName) const;
  void setFormula(const OT::String& outputName, const OT::String& formula);

  OT::String getHtmlDescription(const bool deterministic) const override;
  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Function generateFunction(const OT::Description & outputNames) const override;

private:
  std::map<OT::String, OT::String> formulaForEachOutput_;
};
}
#endif
