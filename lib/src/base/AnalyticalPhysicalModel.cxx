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
#include "otgui/AnalyticalPhysicalModel.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(AnalyticalPhysicalModel);

static Factory<AnalyticalPhysicalModel> RegisteredFactory;

/* Default constructor */
AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
}


/* Constructor with parameters */
AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String & name,
                                                 const InputCollection & inputs,
                                                 const OutputCollection & outputs)
  : PhysicalModelImplementation(name, inputs, outputs)
{
}


/* Virtual constructor */
AnalyticalPhysicalModel* AnalyticalPhysicalModel::clone() const
{
  return new AnalyticalPhysicalModel(*this);
}


Description AnalyticalPhysicalModel::getFormulas() const
{
  Description formulas(getOutputs().getSize());
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    formulas[i] = getOutputs()[i].getFormula();
  return formulas;
}


NumericalMathFunction AnalyticalPhysicalModel::getFunction() const
{
  if (!getInputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no outputs.";

  return NumericalMathFunction(getInputNames(), getOutputNames(), getFormulas());
}


String AnalyticalPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = [";
  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    oss << getInputs()[i].getName();
    if (i < getInputs().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputs = [";
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
  {
    oss << getOutputs()[i].getName();
    if (i < getOutputs().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << getName()+ " = otguibase.AnalyticalPhysicalModel('" + getName() + "', inputs, outputs)\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/* String converter */
String AnalyticalPhysicalModel::__repr__() const
{
  return PhysicalModelImplementation::__repr__();
}


/* Method save() stores the object through the StorageManager */
void AnalyticalPhysicalModel::save(Advocate & adv) const
{
  PhysicalModelImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void AnalyticalPhysicalModel::load(Advocate & adv)
{
  PhysicalModelImplementation::load(adv);
}
}