//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models
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
#ifndef PERSALYS_PHYSICALMODEL_HXX
#define PERSALYS_PHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API PhysicalModel : public OT::TypedInterfaceObject<PhysicalModelImplementation>
{
  CLASSNAME

public:
  typedef OT::Pointer<PhysicalModelImplementation> Implementation;

  /** Default constructor */
  explicit PhysicalModel(const OT::String & name = "Unnamed");
  /** Default constructor */
  PhysicalModel(const PhysicalModelImplementation & implementation);
  /** Constructor from implementation */
  PhysicalModel(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  PhysicalModel(PhysicalModelImplementation * p_implementation);

  /** Virtual constructor */
  virtual PhysicalModel * clone() const;

  /** Comparison operator */
  OT::Bool operator ==(const PhysicalModel & other) const;
  OT::Bool operator !=(const PhysicalModel & other) const;

  void addObserver(Observer * observer);
  void blockNotification(const OT::String & blockedObserverType = "");

  OT::UnsignedInteger getInputDimension() const;
  InputCollection getInputs() const;
  Input & getInputByName(const OT::String & inputName);
  Input getInputByName(const OT::String & inputName) const;
  void setInputs(const InputCollection & inputs);
  void setInputName(const OT::String & inputName, const OT::String & newName);
  void setInputDescription(const OT::String & inputName, const OT::String & description);
  void setInputValue(const OT::String & inputName, const double & value);
  void setInputStochastic(const OT::String & inputName, const bool & stoch);
  void setDistribution(const OT::String & inputName, const OT::Distribution & distribution);
  void setDistributionParametersType(const OT::String & inputName, const OT::UnsignedInteger & distributionParametersType);
  void setFiniteDifferenceStep(const OT::String& inputName, const double& step);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);
  OT::Description getInputNames() const;
  OT::Description getStochasticInputNames() const;
  bool hasInputNamed(const OT::String & inputName) const;
  bool hasStochasticInputs() const;

  OT::UnsignedInteger getOutputDimension() const;
  OutputCollection getOutputs() const;
  Output & getOutputByName(const OT::String & outputName);
  Output getOutputByName(const OT::String & outputName) const;
  void setOutputs(const OutputCollection & outputs);
  void setOutputName(const OT::String & outputName, const OT::String & newName);
  void setOutputDescription(const OT::String & outputName, const OT::String & description);
  void setOutputValue(const OT::String & outputName, const double & value);
  void selectOutput(const OT::String & outputName, const bool selected);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);
  OT::Description getOutputNames() const;
  bool hasOutputNamed(const OT::String & outputName) const;
  OT::Description getSelectedOutputsNames() const;

  OT::Distribution getDistribution() const;
  OT::RandomVector getInputRandomVector() const;
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames) const;

  OT::Function getFunction() const;
  OT::Function getFunction(const OT::Description & outputNames) const;
  OT::Function getFunction(const OT::String & outputName) const;
  OT::Function getRestrictedFunction() const;
  OT::Function getRestrictedFunction(const OT::Description & outputNames) const;
  OT::PointToFieldFunction getPointToFieldFunction() const;
  OT::PointToFieldFunction getPointToFieldFunction(const OT::Description & outputNames) const;
  OT::PointToFieldFunction getRestrictedPointToFieldFunction(const OT::Description & outputNames) const;

  OT::Distribution getCopula() const;
  OT::Collection<OT::Distribution> getCopulaCollection() const;
  void setCopula(const OT::Description& inputNames, const OT::Distribution & copula);

  void setEvalTime(const OT::Scalar& evalTime);
  OT::Scalar getEvalTime() const;

  bool hasMesh() const;
  MeshModel getMeshModel() const;
  void setMeshModel(const MeshModel& meshModel);

  OT::Bool isParallel() const;
  void setParallel(const OT::Bool flag);

  OT::UnsignedInteger getProcessNumber() const;
  void setProcessNumber(const OT::UnsignedInteger processNumber);

  bool isValid() const;

  OT::String getHtmlDescription(const bool deterministic) const;
  OT::String getPythonScript() const;

  void exportStandalonePythonScript(const OT::String & fileName) const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif
