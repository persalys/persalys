//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis
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
#ifndef OTGUI_ANALYSISIMPLEMENTATION_HXX
#define OTGUI_ANALYSISIMPLEMENTATION_HXX

#include "PhysicalModel.hxx"
#include "OTTools.hxx"
#include "LaunchParametersVisitor.hxx"

namespace OTGUI
{
class OTGUI_API AnalysisImplementation : public OT::PersistentObject, public Observable
{
public:
  /** Default constructor */
  AnalysisImplementation();

  /** Constructor with parameters */
  AnalysisImplementation(const OT::String & name);

  /** Virtual constructor */
  virtual AnalysisImplementation * clone() const;

  /** Object name accessor */
  virtual void setName(const OT::String& name);

  /** Comparison operators */
  OT::Bool operator ==(const AnalysisImplementation & other) const;
  OT::Bool operator !=(const AnalysisImplementation & other) const;

  virtual Parameters getParameters() const;
  void run();
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  bool isReliabilityAnalysis() const;

  bool isRunning() const;

  OT::Description getInterestVariables() const;
  virtual void setInterestVariables(const OT::Description& variablesNames);

  OT::String getInformationMessage() const;
  OT::String getErrorMessage() const;
  OT::String getWarningMessage() const;
  int getProgressValue() const;

  virtual void stop();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  virtual void acceptLaunchParameters(LaunchParametersVisitor* visitor);

protected:
  virtual void initialize();
  virtual void launch();

protected:
  bool isReliabilityAnalysis_;
  OT::String informationMessage_;
  OT::String warningMessage_;
  bool stopRequested_;
  int progressValue_;

private:
  bool isRunning_;
  OT::String errorMessage_;
  OT::Description interestVariables_;
};
}
#endif
