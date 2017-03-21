//                                               -*- C++ -*-
/**
 *  @brief Class to define outputs of the physical models
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
#ifndef OTGUI_OUTPUT_HXX
#define OTGUI_OUTPUT_HXX

#include "Variable.hxx"
#include "openturns/Collection.hxx"

namespace OTGUI {
class OTGUI_API Output : public Variable
{
  CLASSNAME;

public:
  /** Default constructor */
  Output();
  /** Constructor with parameters */
  Output(const OT::String& name, const OT::String& description="");

  /** Virtual constructor */
  virtual Output * clone() const;

  bool isSelected() const;
  void setIsSelected(const bool isSelected);

  bool hasBeenComputed() const;
  void setHasBeenComputed(const bool hasBeenComputed);

  OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  bool isSelected_;
  bool hasBeenComputed_;
};
typedef OT::Collection<Output> OutputCollection;
}
#endif