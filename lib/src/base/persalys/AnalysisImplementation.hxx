//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_ANALYSISIMPLEMENTATION_HXX
#define PERSALYS_ANALYSISIMPLEMENTATION_HXX

#include "PhysicalModel.hxx"
#include "BaseTools.hxx"
#include "LaunchParametersVisitor.hxx"

class Study;
namespace PERSALYS
{
class PERSALYS_API AnalysisImplementation : public OT::PersistentObject, public Observable
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

  virtual void removeAllObservers();
  virtual Observer * getParentObserver() const;

  /** Comparison operators */
  OT::Bool operator ==(const AnalysisImplementation & other) const;
  OT::Bool operator !=(const AnalysisImplementation & other) const;

  virtual Parameters getParameters() const;
  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;
  virtual bool canBeLaunched(OT::String &errorMessage) const;

  bool isReliabilityAnalysis() const;

  bool isRunning() const;

  OT::Description getInterestVariables() const;
  virtual void setInterestVariables(const OT::Description& variablesNames);

  OT::String getInformationMessage() const;
  OT::String getErrorMessage() const;
  OT::String getWarningMessage() const;
  int getProgressValue() const;
  OT::String getHtmlDescription() const;
  OT::Scalar getElapsedTime() const;
  virtual void stop();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  virtual void acceptLaunchParameters(LaunchParametersVisitor* visitor);

protected:
  virtual void initialize();
  virtual void launch();
  static bool Stop(void * p);
  static void UpdateProgressValue(double percent, void * data);

protected:
  bool isReliabilityAnalysis_;
  bool isDeterministicAnalysis_;
  OT::String informationMessage_;
  OT::String warningMessage_;
  bool stopRequested_;
  int progressValue_;
  OT::String modelHtmlDescription_;

private:
  OT::Scalar elapsedTime_;
  bool isRunning_;
  OT::String errorMessage_;
  OT::Description interestVariables_;
};
}
#endif
