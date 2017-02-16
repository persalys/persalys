//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_ANALYTICALPHYSICALMODEL_HXX
#define OTGUI_ANALYTICALPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class OTGUI_API AnalyticalPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  /** Default constructor */
  AnalyticalPhysicalModel(const OT::String& name="Unnamed");
  /** Constructor with parameters */
  AnalyticalPhysicalModel(const OT::String& name,
                          const InputCollection& inputs,
                          const OutputCollection& outputs,
                          const OT::Description& formulas);

  /** Virtual constructor */
  virtual AnalyticalPhysicalModel * clone() const;

  virtual void addOutput(const Output& output);
  virtual void removeOutput(const OT::String& outputName);
  virtual void setOutputs(const OutputCollection& outputs);
  virtual void setOutputName(const OT::String& outputName, const OT::String& newName);

  OT::Description getFormulas() const;
  void setFormulas(const OT::Description& formulas);

  OT::String getFormula(const OT::String& outputName) const;
  void setFormula(const OT::String& outputName, const OT::String& formula);

  virtual OT::NumericalMathFunction getFunction() const;

  OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(OT::Advocate & adv);

private:
  std::map<OT::String, OT::String> formulaForEachOutput_;
};
}
#endif