//                                               -*- C++ -*-
/**
 *  @brief The class that implements the variable of the physical models
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
class PERSALYS_BASE_API Variable : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  Variable();
  /** Constructor with parameters */
  Variable(const OT::String & name, const double & value, const OT::String & description, const OT::String & unit = "");
  Variable(const OT::String & name, const OT::String & description = "");

  /** Virtual constructor */
  Variable * clone() const override;

  /** Comparison operator */
  OT::Bool operator ==(const Variable & other) const;

  /** String converter */
  OT::String __repr__() const override;

  virtual void setName(const OT::String &name);

  double getValue() const;
  void setValue(const double & value);

  OT::String getDescription() const;
  void setDescription(const OT::String & description);

  OT::String getUnit() const;
  void setUnit(const OT::String & unit);

  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::String getEscapedDescription() const;

private:
  double value_ = 0.0;
  OT::String description_;
  OT::String unit_;
};
typedef OT::Collection<Variable> VariableCollection;
}
#endif
