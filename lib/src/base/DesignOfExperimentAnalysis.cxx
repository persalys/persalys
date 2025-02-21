//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all analysis with design of experiments
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/DesignOfExperimentAnalysis.hxx"

using namespace OT;

namespace PERSALYS
{

/* Default constructor */
DesignOfExperimentAnalysis::DesignOfExperimentAnalysis()
  : AnalysisImplementation()
  , designOfExperiment_()
{
}


/* Constructor with parameters */
DesignOfExperimentAnalysis::DesignOfExperimentAnalysis(const String & name)
  : AnalysisImplementation(name)
  , designOfExperiment_()
{
}


/* Constructor with parameters */
DesignOfExperimentAnalysis::DesignOfExperimentAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
  : AnalysisImplementation(name)
  , designOfExperiment_(designOfExperiment)
{
}


/* Virtual constructor */
DesignOfExperimentAnalysis* DesignOfExperimentAnalysis::clone() const
{
  return new DesignOfExperimentAnalysis(*this);
}


Observer * DesignOfExperimentAnalysis::getParentObserver() const
{
  if (Observer * obs = designOfExperiment_.getImplementation()->getObserver("DataModelDiagramItem"))
    return obs;
  return designOfExperiment_.getImplementation()->getObserver("DesignOfExperimentDefinitionItem");
}


DesignOfExperiment DesignOfExperimentAnalysis::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


bool DesignOfExperimentAnalysis::canBeLaunched(String &errorMessage) const
{
  // doe must be not empty
  if (!getDesignOfExperiment().getSample().getSize())
    errorMessage = "The sample must be not empty.";
  // values must be valid
  if (!getDesignOfExperiment().isValid())
    errorMessage = "The sample contains invalid values.";

  return errorMessage.empty();
}


void DesignOfExperimentAnalysis::run()
{
  AnalysisImplementation::run();
  if (designOfExperiment_.hasPhysicalModel())
    modelHtmlDescription_ = designOfExperiment_.getPhysicalModel().getHtmlDescription(false);
}


String DesignOfExperimentAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " name=" << getName()
      << " designOfExperiment=" << getDesignOfExperiment().getName()
      << " interestVariables=" << getInterestVariables();
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
  if (!getInterestVariables().getSize())
    setInterestVariables(designOfExperiment_.getOutputSample().getDescription());
}


void DesignOfExperimentAnalysis::acceptLaunchParameters(LaunchParametersVisitor* visitor)
{
  if(designOfExperiment_.hasPhysicalModel())
  {
    designOfExperiment_.getPhysicalModel().getImplementation()->acceptLaunchParameters(visitor);
  }
}
}
