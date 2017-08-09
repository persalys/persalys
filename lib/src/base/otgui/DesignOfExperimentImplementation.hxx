//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTIMPLEMENTATION_HXX
#define OTGUI_DESIGNOFEXPERIMENTIMPLEMENTATION_HXX

#include "PhysicalModel.hxx"
#include "DataSample.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentImplementation : public DataSample, public Observable
{
  CLASSNAME;

public:
  /** Default constructor */
  DesignOfExperimentImplementation();
  /** Constructor with parameters */
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual DesignOfExperimentImplementation * clone() const;

  /** Comparison operators */
  OT::Bool operator ==(const DesignOfExperimentImplementation & other) const;
  OT::Bool operator !=(const DesignOfExperimentImplementation & other) const;

  bool hasPhysicalModel() const;

  PhysicalModel getPhysicalModel() const;

  virtual OT::Description getVariableInputNames() const;

  OT::Sample getFailedInputSample() const;
  OT::Sample getNotEvaluatedInputSample() const;

  virtual OT::Sample getOriginalInputSample() const;
  void setOriginalInputSample(const OT::Sample & sample);

  virtual void setInputSample(const OT::Sample & sample);

  OT::String getErrorMessage() const;
  int getProgressValue() const;

  OT::UnsignedInteger getBlockSize() const;
  void setBlockSize(const OT::UnsignedInteger size);

  OT::Description getInterestVariables() const;
  void setInterestVariables(const OT::Description& variablesNames);

  void initialize();

  void run();
  virtual OT::String getPythonScript() const;
  void stop();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  bool hasPhysicalModel_;
  PhysicalModel physicalModel_;
  OT::Description interestVariables_;
  OT::String errorMessage_;
  bool stopRequested_;
  int progressValue_;
  mutable OT::Sample originalInputSample_;
  mutable OT::Sample failedInputSample_;
  OT::UnsignedInteger blockSize_;
};
}
#endif
