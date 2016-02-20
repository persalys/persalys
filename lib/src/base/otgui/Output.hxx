//                                               -*- C++ -*-
/**
 *  @brief Class to define outputs of the physical models
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
#ifndef OTGUI_OUTPUT_HXX
#define OTGUI_OUTPUT_HXX

#include "Variable.hxx"
#include "Collection.hxx"

namespace OTGUI {
class Output : public Variable
{
  CLASSNAME;

public:
  /** Default constructor */
  Output();
  /** Constructor with parameters */
  Output(const OT::String & name, const double & value=0., const OT::String & description="",
         const OT::String & formula="");

  /** Virtual constructor */
  virtual Output * clone() const;

  OT::String getFormula() const;
  void setFormula(const OT::String & formula);

  bool hasBeenComputed() const;
  void setHasBeenComputed(const bool hasBeenComputed);

  OT::String getPythonScript() const; 

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::String formula_;
  bool hasBeenComputed_;
};
typedef OT::Collection<Output> OutputCollection;
}
#endif