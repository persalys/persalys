//                                               -*- C++ -*-
/**
 *  @brief The class that implements the variable of the physical models
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
#ifndef PERSALYS_VARIABLE_HXX
#define PERSALYS_VARIABLE_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/OTType.hxx>

namespace PERSALYS
{
class PERSALYS_API Variable : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  Variable();
  /** Constructor with parameters */
  Variable(const OT::String & name, const double & value, const OT::String & description);
  Variable(const OT::String & name, const OT::String & description = "");

  /** Virtual constructor */
  virtual Variable * clone() const;

  /** String converter */
  virtual OT::String __repr__() const;

  virtual void setName(const OT::String &name);

  double getValue() const;
  void setValue(const double & value);

  OT::String getDescription() const;
  void setDescription(const OT::String & description);

  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(OT::Advocate & adv);

protected:
  OT::String getEscapedDescription() const;

private:
  double value_;
  OT::String description_;
};
typedef OT::Collection<Variable> VariableCollection;
}
#endif
