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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_MODELEVALUATION_HXX
#define OTGUI_MODELEVALUATION_HXX

#include "PhysicalModelAnalysis.hxx"
#include "DesignOfExperiment.hxx"

namespace OTGUI
{
class OTGUI_API ModelEvaluation : public PhysicalModelAnalysis
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

  void updateParameters();

  OT::Point getInputValues() const;
  void setInputValue(const OT::UnsignedInteger index, const double value);

  DesignOfExperiment getDesignOfExperiment() const;
  OT::Point getOutputValues() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void initialize();
  virtual void launch();
  void initializeParameters();

private:
  OT::Description inputNames_;
  OT::Point inputValues_;
  DesignOfExperiment designOfExperiment_;
};
}
#endif
