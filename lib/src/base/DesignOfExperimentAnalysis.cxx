//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all analysis with design of experiment
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
#include "otgui/DesignOfExperimentAnalysis.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
DesignOfExperimentAnalysis::DesignOfExperimentAnalysis()
  : AnalysisImplementation()
{
}


/* Constructor with parameters */
DesignOfExperimentAnalysis::DesignOfExperimentAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
  : AnalysisImplementation(name)
  , designOfExperiment_(designOfExperiment)
{
  setOutputsToAnalyse(designOfExperiment_.getOutputSample().getDescription());
}


/* Virtual constructor */
DesignOfExperimentAnalysis* DesignOfExperimentAnalysis::clone() const
{
  return new DesignOfExperimentAnalysis(*this);
}


DesignOfExperiment DesignOfExperimentAnalysis::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


void DesignOfExperimentAnalysis::setDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  designOfExperiment_ = designOfExperiment;
  setOutputsToAnalyse(designOfExperiment.getOutputSample().getDescription());
}


String DesignOfExperimentAnalysis::getModelName() const
{
  return designOfExperiment_.getName();
}


void DesignOfExperimentAnalysis::setOutputsToAnalyse(const Description& outputsNames)
{
  const Description modelOutputsNames(designOfExperiment_.getOutputSample().getDescription());
  for (UnsignedInteger i=0; i<outputsNames.getSize(); ++i)
    if (!modelOutputsNames.contains(outputsNames[i]))
      throw InvalidArgumentException(HERE) << "The name " << outputsNames[i] << " does not match an output name of the model";

  AnalysisImplementation::setOutputsToAnalyse(outputsNames);
}


String DesignOfExperimentAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " designOfExperiment=class=" << getDesignOfExperiment().GetClassName() << " name=" << getDesignOfExperiment().getName();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentAnalysis::save(Advocate & adv) const
{
  AnalysisImplementation::save(adv);
  adv.saveAttribute("designOfExperiment_", designOfExperiment_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentAnalysis::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
  adv.loadAttribute("designOfExperiment_", designOfExperiment_);
  if (!getOutputsToAnalyse().getSize())
    setOutputsToAnalyse(designOfExperiment_.getOutputSample().getDescription());
}
}