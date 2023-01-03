//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_PHYSICALMODELIMPLEMENTATION_HXX
#define PERSALYS_PHYSICALMODELIMPLEMENTATION_HXX

#include "Input.hxx"
#include "Output.hxx"
#include "Observable.hxx"
#include "MeshModel.hxx"
#include "LaunchParametersVisitor.hxx"

#include <openturns/Function.hxx>
#include <openturns/PointToFieldFunction.hxx>
#include <openturns/RandomVector.hxx>
#include <openturns/ComposedDistribution.hxx>
#include <openturns/ComposedCopula.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API PhysicalModelImplementation : public OT::PersistentObject, public Observable
{
public:
  /** Constructor with parameters */
  explicit PhysicalModelImplementation(const OT::String & name);

  /** Constructor with parameters */
  PhysicalModelImplementation(const OT::String & name,
                              const InputCollection & inputs,
                              const OutputCollection & outputs);

  /** Virtual constructor */
  PhysicalModelImplementation * clone() const override;

  virtual void setName(const OT::String & name);

  void removeAllObservers() override;

  /** Comparison operators */
  OT::Bool operator ==(const PhysicalModelImplementation & other) const;
  OT::Bool operator !=(const PhysicalModelImplementation & other) const;

  OT::UnsignedInteger getInputDimension() const;
  InputCollection getInputs() const;
  Input & getInputByName(const OT::String & inputName);
  Input getInputByName(const OT::String & inputName) const;
  virtual void setInputs(const InputCollection & inputs);
  virtual void setInputName(const OT::String & inputName, const OT::String & newName);
  void setInputDescription(const OT::String & inputName, const OT::String & description);
  void setInputValue(const OT::String & inputName, const double & value);
  void setInputStochastic(const OT::String & inputName, const bool & stoch);
  void setDistribution(const OT::String & inputName, const OT::Distribution & distribution);
  void setDistributionParametersType(const OT::String & inputName, const OT::UnsignedInteger & distributionParametersType);
  void setFiniteDifferenceStep(const OT::String& inputName, const double& step);
  virtual void addInput(const Input & input);
  virtual void removeInput(const OT::String & inputName);
  void clearInputs();
  OT::Description getInputNames() const;
  OT::Description getStochasticInputNames() const;
  bool hasInputNamed(const OT::String & inputName) const;
  bool hasStochasticInputs() const;

  OT::UnsignedInteger getOutputDimension() const;
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
  void clearOutputs();
  OT::Description getOutputNames() const;
  bool hasOutputNamed(const OT::String & outputName) const;
  OT::Description getSelectedOutputsNames() const;

  OT::Distribution getDistribution() const;
  OT::RandomVector getInputRandomVector() const;
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames) const;

  OT::Function getFunction() const;
  OT::Function getFunction(const OT::Description& outputNames) const;
  OT::Function getFunction(const OT::String & outputName) const;
  OT::Function getRestrictedFunction() const;
  OT::Function getRestrictedFunction(const OT::Description & outputNames) const;
  OT::PointToFieldFunction getPointToFieldFunction() const;
  OT::PointToFieldFunction getPointToFieldFunction(const OT::Description & outputNames) const;
  OT::PointToFieldFunction getRestrictedPointToFieldFunction(const OT::Description & outputNames) const;

  OT::Distribution getCopula() const;
  OT::Collection<OT::Distribution> getCopulaCollection() const;
  void setCopula(const OT::Description& inputNames, const OT::Distribution & copula);

  bool hasMesh() const;
  MeshModel getMeshModel() const;
  void setMeshModel(const MeshModel& meshModel);

  OT::Scalar getEvalTime() const;
  void setEvalTime(const OT::Scalar& evalTime);

  OT::Bool isParallel() const;
  virtual void setParallel(const OT::Bool flag);

  OT::UnsignedInteger getProcessNumber() const;
  void setProcessNumber(const OT::UnsignedInteger processNumber);

  bool isValid() const;

  virtual OT::String getHtmlDescription(const bool deterministic) const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

  virtual void acceptLaunchParameters(LaunchParametersVisitor* visitor);

protected:
  virtual OT::Function generateFunction(const OT::Description & outputNames) const;
  virtual OT::PointToFieldFunction generatePointToFieldFunction(const OT::Description & outputNames) const;

  void updateCopula();
  OT::String getProbaModelPythonScript() const;
  OT::String getCopulaPythonScript() const;
  OT::Point getFiniteDifferenceSteps() const;
  void updateFiniteDifferenceSteps() const;

private:
  void inputsChanged();

protected:
  bool hasMesh_ = false;
private:
  OT::PersistentCollection<Input> inputs_;
  OT::PersistentCollection<Output> outputs_;
  mutable OT::ComposedCopula composedCopula_;
  MeshModel meshModel_;
  mutable OT::Point finiteDifferenceSteps_;
  OT::Bool isParallel_ = false;
  OT::UnsignedInteger processNumber_ = 1;
  OT::Scalar evalTime_ = 0.0;
};
}
#endif
