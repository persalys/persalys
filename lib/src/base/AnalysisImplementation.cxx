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
#include "persalys/AnalysisImplementation.hxx"

#include "persalys/PhysicalModelAnalysis.hxx"
#include "persalys/PythonPhysicalModel.hxx"

using namespace OT;

namespace PERSALYS
{

/* Default constructor */
AnalysisImplementation::AnalysisImplementation()
  : PersistentObject()
  , Observable()
  , isReliabilityAnalysis_(false)
  , isDeterministicAnalysis_(true)
  , informationMessage_("")
  , warningMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
  , modelHtmlDescription_("")
  , isRunning_(false)
  , errorMessage_("")
  , interestVariables_()
{
}


/* Constructor with parameters */
AnalysisImplementation::AnalysisImplementation(const String & name)
  : PersistentObject()
  , Observable()
  , isReliabilityAnalysis_(false)
  , isDeterministicAnalysis_(true)
  , informationMessage_("")
  , warningMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
  , modelHtmlDescription_("")
  , isRunning_(false)
  , errorMessage_("")
  , interestVariables_()
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
  notify("nameChanged");
}


void AnalysisImplementation::removeAllObservers()
{
  notifyAndRemove("AnalysisItem");
  notifyAndRemove("Study");
}


Observer * AnalysisImplementation::getParentObserver() const
{
  return 0;
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


void AnalysisImplementation::initialize()
{
  informationMessage_ = "";
  errorMessage_ = "";
  warningMessage_ = "";
  stopRequested_ = false;
  progressValue_ = 0;
  modelHtmlDescription_ = "";

  // if the model is a PythonPhysicalModel: reset the calls number
  // do not need this operation for the other models because
  // the Evaluation object is built each time getFunction() is called
  if (PhysicalModelAnalysis * pmAnalysis = dynamic_cast<PhysicalModelAnalysis*>(this))
  {
    PythonPhysicalModel * model = dynamic_cast<PythonPhysicalModel*>(pmAnalysis->getPhysicalModel().getImplementation().get());
    if (model)
    {
      model->resetCallsNumber();
    }
  }
}


void AnalysisImplementation::launch()
{
}

Scalar AnalysisImplementation::getElapsedTime() const
{
  return elapsedTime_;
}
void AnalysisImplementation::run()
{
  TimeCriteria tc;
  isRunning_ = true;
  notify("analysisLaunched");
  try
  {
    initialize();

    launch();

    isRunning_ = false;
    tc.incrementElapsedTime();
    elapsedTime_ = tc.getElapsedTime();
    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ +=  ex.what();
    isRunning_ = false;
    tc.incrementElapsedTime();
    elapsedTime_ = tc.getElapsedTime();
    notify("analysisBadlyFinished");
    throw;
  }
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


bool AnalysisImplementation::canBeLaunched(String & /*errorMessage*/) const
{
  return true;
}


String AnalysisImplementation::getPythonScript() const
{
  return "";
}


String AnalysisImplementation::getHtmlDescription() const
{
  return modelHtmlDescription_;
}


void AnalysisImplementation::stop()
{
  stopRequested_ = true;
}


bool AnalysisImplementation::Stop(void * p)
{
  AnalysisImplementation * arg = (AnalysisImplementation*)p;
  return arg->stopRequested_;
}


void AnalysisImplementation::UpdateProgressValue(double percent, void * data)
{
  AnalysisImplementation * analysis = static_cast<AnalysisImplementation*>(data);
  if (!analysis)
    return;

  // set progress value
  analysis->progressValue_ = (int) percent;
  analysis->notify("progressValueChanged");
}


/* Method save() stores the object through the StorageManager */
void AnalysisImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("errorMessage_", errorMessage_);
  adv.saveAttribute("warningMessage_", warningMessage_);
  adv.saveAttribute("isReliabilityAnalysis_", isReliabilityAnalysis_);
  adv.saveAttribute("isDeterministicAnalysis_", isDeterministicAnalysis_);
  adv.saveAttribute("interestVariables_", interestVariables_);
  adv.saveAttribute("modelHtmlDescription_", modelHtmlDescription_);
}


/* Method load() reloads the object from the StorageManager */
void AnalysisImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("errorMessage_", errorMessage_);
  adv.loadAttribute("warningMessage_", warningMessage_);
  adv.loadAttribute("isReliabilityAnalysis_", isReliabilityAnalysis_);
  adv.loadAttribute("isDeterministicAnalysis_", isDeterministicAnalysis_);
  adv.loadAttribute("interestVariables_", interestVariables_);
  adv.loadAttribute("modelHtmlDescription_", modelHtmlDescription_);
}


/* To be implemented in child classes. */
void AnalysisImplementation::acceptLaunchParameters(LaunchParametersVisitor* /*visitor*/)
{
}
}
