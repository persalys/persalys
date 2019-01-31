//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_MODELEVALUATION_HXX
#define OTGUI_MODELEVALUATION_HXX

#include "GridDesignOfExperiment.hxx"

namespace OTGUI
{
class OTGUI_API ModelEvaluation : public GridDesignOfExperiment
{
  CLASSNAME

public:
  /** Default constructor */
  ModelEvaluation();
  /** Constructor with parameters */
  ModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel);
  ModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel,
                  const OT::Point & inputsValues);

  /** Virtual constructor */
  virtual ModelEvaluation * clone() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void launch();
};
}
#endif
