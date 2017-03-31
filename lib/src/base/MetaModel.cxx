//                                               -*- C++ -*-
/**
 *  @brief MetaModel implements models created by MetaModelAnalysis
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

#include "otgui/MetaModel.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MetaModel);

static Factory<MetaModel> RegisteredFactory;

/* Default constructor */
MetaModel::MetaModel(const String& name)
  : PhysicalModelImplementation(name)
{
}


/* Constructor with parameters */
MetaModel::MetaModel(const OT::String& name, const OT::NumericalMathFunction& function)
  : PhysicalModelImplementation(name)
  , function_(function)
{
}


MetaModel* MetaModel::clone() const
{
  return new MetaModel(*this);
}


NumericalMathFunction MetaModel::generateFunction(const Description & outputNames) const
{
  return function_;
}


void MetaModel::setFunction(const NumericalMathFunction& function)
{
  function_ = function;
}


String MetaModel::getPythonScript() const
{
  return "";
}


/* String converter */
String MetaModel::__repr__() const
{
  return PhysicalModelImplementation::__repr__();
}


/* Method save() stores the object through the StorageManager */
void MetaModel::save(Advocate& adv) const
{
  PhysicalModelImplementation::save(adv);
  adv.saveAttribute("function_", function_);
}


/* Method load() reloads the object from the StorageManager */
void MetaModel::load(Advocate& adv)
{
  PhysicalModelImplementation::load(adv);
  adv.loadAttribute("function_", function_);
}
}
