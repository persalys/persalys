//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
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
#include "otgui/AnalyticalPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(AnalyticalPhysicalModel);

AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
}


AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String & name,
                                                 const InputCollection & inputs,
                                                 const OutputCollection & outputs)
  : PhysicalModelImplementation(name, inputs, outputs)
{
}


AnalyticalPhysicalModel::AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other)
  : PhysicalModelImplementation(other)
{
}


AnalyticalPhysicalModel* AnalyticalPhysicalModel::clone() const
{
  return new AnalyticalPhysicalModel(*this);
}


Description AnalyticalPhysicalModel::getFormulas()
{
  Description formulas(getOutputs().getSize());
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    formulas[i] = getOutputs()[i].getFormula();
  return formulas;
}


NumericalMathFunction AnalyticalPhysicalModel::getFunction()
{
  return NumericalMathFunction(getInputNames(), getOutputNames(), getFormulas());
}


String AnalyticalPhysicalModel::dump() const
{
  String result;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    result += getInputs()[i].dump();

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    result += getOutputs()[i].dump();

  result += getName()+ " = otguibase.AnalyticalPhysicalModel('" + getName() + "')\n";

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    result += getName()+ ".addInput(" + getInputs()[i].getName() + ")\n";

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    result += getName()+ ".addOutput(" + getOutputs()[i].getName() + ")\n";

  result += PhysicalModelImplementation::dumpCopula();
  return result;
}
}