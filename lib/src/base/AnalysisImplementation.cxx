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
#include "otgui/AnalysisImplementation.hxx"

using namespace OT;

namespace OTGUI
{

/* Default constructor */
AnalysisImplementation::AnalysisImplementation()
  : PersistentObject()
  , Observable()
  , isReliabilityAnalysis_(false)
  , isRunning_(false)
  , informationMessage_("")
  , errorMessage_("")
  , warningMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
{
}


/* Constructor with parameters */
AnalysisImplementation::AnalysisImplementation(const String & name)
  : PersistentObject()
  , Observable()
  , isReliabilityAnalysis_(false)
  , isRunning_(false)
  , informationMessage_("")
  , errorMessage_("")
  , warningMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
{
  setName(name);
}


/* Virtual constructor */
AnalysisImplementation* AnalysisImplementation::clone() const
{
  return new AnalysisImplementation(*this);
}


/* Object name accessor */
void AnalysisImplementation::setName(const String& name)
{
  PersistentObject::setName(name);
}


Bool AnalysisImplementation::operator==(const AnalysisImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


Bool AnalysisImplementation::operator!=(const AnalysisImplementation& other) const
{
  return !operator==(other);
}


void AnalysisImplementation::initialize()
{
  informationMessage_ = "";
  errorMessage_ = "";
  warningMessage_ = "";
  stopRequested_ = false;
  progressValue_ = 0;
}


bool AnalysisImplementation::isReliabilityAnalysis() const
{
  return isReliabilityAnalysis_;
}


bool AnalysisImplementation::isRunning() const
{
  return isRunning_;
}


Description AnalysisImplementation::getInterestVariables() const
{
  return interestVariables_;
}


void AnalysisImplementation::setInterestVariables(const Description& variablesNames)
{
  interestVariables_ = variablesNames;
}


Parameters AnalysisImplementation::getParameters() const
{
  return Parameters();
}


void AnalysisImplementation::run()
{
}


String AnalysisImplementation::getInformationMessage() const
{
  return informationMessage_;
}


String AnalysisImplementation::getErrorMessage() const
{
  return errorMessage_;
}


String AnalysisImplementation::getWarningMessage() const
{
  return warningMessage_;
}


int AnalysisImplementation::getProgressValue() const
{
  return progressValue_;
}


bool AnalysisImplementation::hasValidResult() const
{
  return false;
}


String AnalysisImplementation::getPythonScript() const
{
  return "";
}


void AnalysisImplementation::stop()
{
  stopRequested_ = true;
}


/* Method save() stores the object through the StorageManager */
void AnalysisImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("errorMessage_", errorMessage_);
  adv.saveAttribute("warningMessage_", warningMessage_);
  adv.saveAttribute("isReliabilityAnalysis_", isReliabilityAnalysis_);
  adv.saveAttribute("interestVariables_", interestVariables_);
}


/* Method load() reloads the object from the StorageManager */
void AnalysisImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("errorMessage_", errorMessage_);
  adv.loadAttribute("warningMessage_", warningMessage_);
  adv.loadAttribute("isReliabilityAnalysis_", isReliabilityAnalysis_);
  adv.loadAttribute("interestVariables_", interestVariables_);
}


/* To be implemented in child classes. */
void AnalysisImplementation::acceptLaunchParameters(LaunchParametersVisitor* visitor)
{
}
}
