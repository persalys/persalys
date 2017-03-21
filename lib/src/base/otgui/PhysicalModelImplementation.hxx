//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models 
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
#ifndef OTGUI_PHYSICALMODELIMPLEMENTATION_HXX
#define OTGUI_PHYSICALMODELIMPLEMENTATION_HXX

#include "Input.hxx"
#include "Output.hxx"
#include "Observable.hxx"

#include <openturns/NumericalMathFunction.hxx>
#include <openturns/RandomVector.hxx>
#include <openturns/ComposedDistribution.hxx>

namespace OTGUI {
class OTGUI_API PhysicalModelImplementation : public OT::PersistentObject, public Observable
{
public:
  /** Constructor with parameters */
  explicit PhysicalModelImplementation(const OT::String & name);

  /** Constructor with parameters */
  PhysicalModelImplementation(const OT::String & name,
                              const InputCollection & inputs,
                              const OutputCollection & outputs);

  /** Virtual constructor */
  virtual PhysicalModelImplementation * clone() const;

  InputCollection getInputs() const;
  Input & getInputByName(const OT::String & inputName);
  Input getInputByName(const OT::String & inputName) const;
  virtual void setInputs(const InputCollection & inputs);
  void setInputName(const OT::String & inputName, const OT::String & newName);
  void setInputDescription(const OT::String & inputName, const OT::String & description);
  void setInputValue(const OT::String & inputName, const double & value);
  void setDistribution(const OT::String & inputName, const OT::Distribution & distribution);
  void setDistributionParametersType(const OT::String & inputName, const OT::UnsignedInteger & distributionParametersType);
  void setFiniteDifferenceStep(const OT::String& inputName, const double& step);
  virtual void addInput(const Input & input);
  virtual void removeInput(const OT::String & inputName);
  OT::Description getInputNames() const;
  OT::Description getStochasticInputNames() const;
  bool hasInputNamed(const OT::String & inputName) const;
  bool hasStochasticInputs() const;

  OutputCollection getOutputs() const;
  Output & getOutputByName(const OT::String & outputName);
  Output getOutputByName(const OT::String & outputName) const;
  virtual void setOutputs(const OutputCollection & outputs);
  virtual void setOutputName(const OT::String & outputName, const OT::String & newName);
  void setOutputDescription(const OT::String & outputName, const OT::String & description);
  void setOutputValue(const OT::String & outputName, const double & value);
  void selectOutput(const OT::String & outputName, const bool selected);
  virtual void addOutput(const Output & output);
  virtual void removeOutput(const OT::String & outputName);
  OT::Description getOutputNames() const;
  bool hasOutputNamed(const OT::String & outputName) const;
  OT::Description getSelectedOutputsNames() const;

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector() const;
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames) const;

  OT::NumericalMathFunction getFunction() const;
  OT::NumericalMathFunction getFunction(const OT::Description& outputNames) const;
  OT::NumericalMathFunction getFunction(const OT::String & outputName) const;
  OT::NumericalMathFunction getRestrictedFunction() const;
  OT::NumericalMathFunction getRestrictedFunction(const OT::Description & outputNames) const;

  OT::Copula getCopula() const;
  void setCopula(const OT::Copula & copula);

  bool isValid() const;

  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual OT::NumericalMathFunction generateFunction() const;
  virtual OT::NumericalMathFunction generateFunction(const OT::Description& outputNames) const;
  void updateCopula();
  OT::String getProbaModelPythonScript() const;
  OT::String getCopulaPythonScript() const;
  OT::NumericalPoint getFiniteDifferenceSteps() const;
  void updateFiniteDifferenceSteps() const;

private:
  OT::PersistentCollection<Input> inputs_;
  OT::PersistentCollection<Output> outputs_;
  OT::Copula copula_;
  mutable OT::NumericalPoint finiteDifferenceSteps_;
};
}
#endif