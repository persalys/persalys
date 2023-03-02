//                                               -*- C++ -*-
/**
 *  @brief Fitting test result
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
#include "persalys/FittingTestResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FittingTestResult)

static Factory<FittingTestResult> Factory_FittingTestResult;
static Factory<PersistentCollection<TestResult> > Factory_PersistentCollection_TestResult;

/* Default constructor */
FittingTestResult::FittingTestResult()
  : PersistentObject()
  , variableName_("")
{
}


/* Virtual constructor */
FittingTestResult* FittingTestResult::clone() const
{
  return new FittingTestResult(*this);
}


String FittingTestResult::getVariableName() const
{
  return variableName_;
}


Sample FittingTestResult::getValues() const
{
  return values_;
}


Collection< Distribution > FittingTestResult::getTestedDistributions() const
{
  return testedDistributions_;
}


Collection< Interval > FittingTestResult::getParamConfidenceInterval() const
{
  return paramCI_;
}


Collection< TestResult > FittingTestResult::getKolmogorovTestResults() const
{
  return kolmogorovTestResults_;
}


Point FittingTestResult::getBICResults() const
{
  return bicResults_;
}


Description FittingTestResult::getErrorMessages() const
{
  return errorMessages_;
}


/* String converter */
String FittingTestResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " variableName=" << getVariableName()
      << " testedDistributions=" << getTestedDistributions()
      << " paramConfidenceInterval=" << getParamConfidenceInterval()
      << " kolmogorovTestResults=" << getKolmogorovTestResults()
      << " bicResults=" << getBICResults()
      << " error messages=" << getErrorMessages();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FittingTestResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("variableName_", variableName_);
  adv.saveAttribute("values_", values_);
  adv.saveAttribute("testedDistributions_", testedDistributions_);
  adv.saveAttribute("paramCI_", paramCI_);
  adv.saveAttribute("kolmogorovTestResults_", kolmogorovTestResults_);
  adv.saveAttribute("bicResults_", bicResults_);
  adv.saveAttribute("errorMessages_", errorMessages_);
}


/* Method load() reloads the object from the StorageManager */
void FittingTestResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("variableName_", variableName_);
  adv.loadAttribute("values_", values_);
  adv.loadAttribute("testedDistributions_", testedDistributions_);
  if (adv.hasAttribute("paramCI_"))
    adv.loadAttribute("paramCI_", paramCI_);
  adv.loadAttribute("kolmogorovTestResults_", kolmogorovTestResults_);
  adv.loadAttribute("bicResults_", bicResults_);
  adv.loadAttribute("errorMessages_", errorMessages_);
}
}
