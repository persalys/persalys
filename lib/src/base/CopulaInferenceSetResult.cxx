//                                               -*- C++ -*-
/**
 *  @brief Copula inference result for a set of variables
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
#include "persalys/CopulaInferenceSetResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CopulaInferenceSetResult)

static Factory<CopulaInferenceSetResult> Factory_CopulaInferenceSetResult;
static Factory<PersistentCollection<ProcessSample > > Factory_CollectionProcessSample;

/* Default constructor */
CopulaInferenceSetResult::CopulaInferenceSetResult()
  : PersistentObject()
{
}


/* Virtual constructor */
CopulaInferenceSetResult* CopulaInferenceSetResult::clone() const
{
  return new CopulaInferenceSetResult(*this);
}


Description CopulaInferenceSetResult::getSetOfVariablesNames() const
{
  return setOfVariablesNames_;
}


Collection< Distribution > CopulaInferenceSetResult::getTestedDistributions() const
{
  return testedDistributions_;
}


Point CopulaInferenceSetResult::getBICResults() const
{
  return bicResults_;
}


PersistentCollection<ProcessSample > CopulaInferenceSetResult::getKendallPlotData() const
{
  return kendallPlotData_;
}


Description CopulaInferenceSetResult::getErrorMessages() const
{
  return errorMessages_;
}


/* String converter */
String CopulaInferenceSetResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " setOfVariablesNames=" << getSetOfVariablesNames()
      << " testedDistributions=" << getTestedDistributions()
      << " BIC results=" << getBICResults()
      << " error messages=" << getErrorMessages();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void CopulaInferenceSetResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("setOfVariablesNames_", setOfVariablesNames_);
  adv.saveAttribute("testedDistributions_", testedDistributions_);
  adv.saveAttribute("kendallPlotDataPS_", kendallPlotData_);
  adv.saveAttribute("bicResults_", bicResults_);
  adv.saveAttribute("errorMessages_", errorMessages_);
}


/* Method load() reloads the object from the StorageManager */
void CopulaInferenceSetResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("setOfVariablesNames_", setOfVariablesNames_);
  adv.loadAttribute("testedDistributions_", testedDistributions_);
  adv.loadAttribute("kendallPlotDataPS_", kendallPlotData_);
  adv.loadAttribute("bicResults_", bicResults_);
  adv.loadAttribute("errorMessages_", errorMessages_);
}
}
