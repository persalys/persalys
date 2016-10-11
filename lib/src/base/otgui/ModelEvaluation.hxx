//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
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
#ifndef OTGUI_MODELEVALUATION_HXX
#define OTGUI_MODELEVALUATION_HXX

#include "PhysicalModelAnalysis.hxx"
#include "ModelEvaluationResult.hxx"

namespace OTGUI {
class OTGUI_API ModelEvaluation : public PhysicalModelAnalysis
{
  CLASSNAME;

public:
  /** Default constructor */
  ModelEvaluation();
  /** Constructor with parameters */
  ModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel);
  ModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel,
                  const OT::NumericalPoint & inputsValues);

  /** Virtual constructor */
  virtual ModelEvaluation * clone() const;

  void updateParameters();

  OT::NumericalPoint getInputValues() const;
  void setInputValue(const int & index, const double & value);

  ModelEvaluationResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters(const InputCollection & inputs);

private:
  OT::Description inputNames_;
  OT::NumericalPoint inputValues_;
  ModelEvaluationResult result_;
};
}
#endif