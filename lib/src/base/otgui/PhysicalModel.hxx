//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models 
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
#ifndef OTGUI_PHYSICALMODEL_HXX
#define OTGUI_PHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class PhysicalModel : public OT::TypedInterfaceObject<PhysicalModelImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<PhysicalModelImplementation> Implementation;

  /** Default constructor */
  explicit PhysicalModel(const OT::String & name="Unnamed");
  /** Constructor with parameters */
  PhysicalModel(const OT::String & name, const InputCollection & inputs, const OutputCollection & outputs);
  /** Default constructor */
  PhysicalModel(const PhysicalModelImplementation & implementation);
  /** Constructor from implementation */
  PhysicalModel(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  PhysicalModel(PhysicalModelImplementation * p_implementation);

  /** Virtual constructor */
  virtual PhysicalModel * clone() const;

  void addObserver(Observer * observer);
  void blockNotification(bool block, const OT::String & notBlockedMessage="");

  InputCollection getInputs() const;
  Input & getInputByName(const OT::String & inputName);
  void setInputs(const InputCollection & inputs);
  void setInputName(const OT::String & inputName, const OT::String & newName);
  void setInputDescription(const OT::String & inputName, const OT::String & description);
  void setInputValue(const OT::String & inputName, const double & value);
  void setInputDistribution(const OT::String & inputName, const OT::Distribution & distribution);
  void setInputDistributionParametersType(const OT::String & inputName, const OT::UnsignedInteger & distributionParametersType);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);
  OT::Description getInputNames();
  OT::Description getStochasticInputNames();
  bool hasInputNamed(const OT::String & inputName);
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output & getOutputByName(const OT::String & outputName);
  void setOutputs(const OutputCollection & outputs);
  void setOutputName(const OT::String & outputName, const OT::String & newName);
  void setOutputDescription(const OT::String & outputName, const OT::String & description);
  void setOutputFormula(const OT::String & outputName, const OT::String & formula);
  void setOutputValue(const OT::String & outputName, const double & value);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);
  OT::Description getOutputNames();
  bool hasOutputNamed(const OT::String & outputName);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames);

  OT::NumericalMathFunction getFunction();
  OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction(const OT::String & outputName);
  OT::NumericalMathFunction getRestrictedFunction();
  OT::NumericalMathFunction getRestrictedFunction(const OT::Description & outputNames);

  OT::Copula getCopula() const;
  void setCopula(const OT::Copula & copula);

  bool isValid() const;

  OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif