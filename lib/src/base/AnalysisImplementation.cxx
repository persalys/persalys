//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis 
 *
 *  Copyright 2015-2016 EDF-Phimeca
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

namespace OTGUI {

/* Default constructor */
AnalysisImplementation::AnalysisImplementation()
  : PersistentObject()
  , Observable()
{
}


/* Constructor with parameters */
AnalysisImplementation::AnalysisImplementation(const String & name)
  : PersistentObject()
  , Observable()
  , isReliabilityAnalysis_(false)
{
  setName(name);
}


/* Virtual constructor */
AnalysisImplementation* AnalysisImplementation::clone() const
{
  return new AnalysisImplementation(*this);
}


String AnalysisImplementation::getModelName() const
{
  return "";
}


bool AnalysisImplementation::isReliabilityAnalysis() const
{
  return isReliabilityAnalysis_;
}


void AnalysisImplementation::setIsReliabilityAnalysis(const bool isReliabilityAnalysis)
{
  isReliabilityAnalysis_ = isReliabilityAnalysis;
}


Description AnalysisImplementation::getOutputsToAnalyse() const
{
  return outputsToAnalyse_;
}


void AnalysisImplementation::setOutputsToAnalyse(const Description& outputsNames)
{
  outputsToAnalyse_ = outputsNames;
}


void AnalysisImplementation::run()
{
}


bool AnalysisImplementation::analysisLaunched() const
{
  return false;
}


String AnalysisImplementation::getPythonScript() const
{
  return "";
}


/* Method save() stores the object through the StorageManager */
void AnalysisImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("isReliabilityAnalysis_", isReliabilityAnalysis_);
  adv.saveAttribute("outputsToAnalyse_", outputsToAnalyse_);
}


/* Method load() reloads the object from the StorageManager */
void AnalysisImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("isReliabilityAnalysis_", isReliabilityAnalysis_);
  adv.loadAttribute("outputsToAnalyse_", outputsToAnalyse_);
}
}