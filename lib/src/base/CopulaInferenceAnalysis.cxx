//                                               -*- C++ -*-
/**
 *  @brief Inference analysis
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
#include "otgui/CopulaInferenceAnalysis.hxx"

#include "otgui/DistributionDictionary.hxx"
#include "otgui/CopulaInferenceSetResult.hxx"

#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/Combinations.hxx>
#include <openturns/VisualTest.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(CopulaInferenceAnalysis)

static Factory<CopulaInferenceAnalysis> Factory_CopulaInferenceAnalysis;
static Factory<PersistentCollection<Description> > Factory_CollectionDescription;

/* Default constructor */
CopulaInferenceAnalysis::CopulaInferenceAnalysis()
  : DesignOfExperimentAnalysis()
  , distFactoriesForSetVar_()
{
}


/* Constructor with parameters */
CopulaInferenceAnalysis::CopulaInferenceAnalysis(const String& name, const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , distFactoriesForSetVar_()
{
  setInterestVariables(designOfExperiment_.getInputSample().getDescription());

  // by default we test the Normal copula for all the inputs of the DOE
  DistributionFactoryCollection collectionCopula;
  collectionCopula.add(NormalCopulaFactory());
  Description allVariables(getInterestVariables());
  allVariables.sort();
  distFactoriesForSetVar_[allVariables] = collectionCopula;
}


/* Virtual constructor */
CopulaInferenceAnalysis* CopulaInferenceAnalysis::clone() const
{
  return new CopulaInferenceAnalysis(*this);
}


CopulaInferenceAnalysis::DistributionFactoryCollection CopulaInferenceAnalysis::getDistributionsFactories(const Description& variablesNames) const
{
  for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i)
    if (!designOfExperiment_.getSample().getDescription().contains(variablesNames[i]))
      throw InvalidArgumentException(HERE) << "Error: the variable name " << variablesNames[i] << " does not match a variable of the model";

  Description setOfVariables(variablesNames);
  setOfVariables.sort();
  std::map<Description, DistributionFactoryCollection>::const_iterator it(distFactoriesForSetVar_.find(setOfVariables));
  if (it == distFactoriesForSetVar_.end())
    throw InvalidArgumentException(HERE) << "Error: no distribution factories set for the set of variables " << variablesNames;

  return it->second;
}


void CopulaInferenceAnalysis::setDistributionsFactories(const Description& variablesNames, const DistributionFactoryCollection& distributionsFactories)
{
  if (!distributionsFactories.getSize())
    throw InvalidArgumentException(HERE) << "Error: the list of distribution factories is empty";

  for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i)
    if (!designOfExperiment_.getSample().getDescription().contains(variablesNames[i]))
      throw InvalidArgumentException(HERE) << "Error: the variable name " << variablesNames[i] << " does not match a variable of the model";

  if (variablesNames.getSize() < 2)
    throw InvalidArgumentException(HERE) << "Error: the dependency inference is performed with at least 2 variables";

  for (UnsignedInteger i = 0; i < distributionsFactories.getSize(); ++i)
    if (distributionsFactories[i].getImplementation()->getClassName().find("Copula") == std::string::npos)
      throw InvalidArgumentException(HERE) << "Error: the dependency inference is performed with copulae.";

  Description setOfVariables(variablesNames);
  setOfVariables.sort();
  distFactoriesForSetVar_[setOfVariables] = distributionsFactories;
}


void CopulaInferenceAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = CopulaInferenceResult();
}


void CopulaInferenceAnalysis::launch()
{
  const UnsignedInteger sizeKendall = 100;

  // for each set:
  std::map<Description, DistributionFactoryCollection>::iterator it;
  for (it = distFactoriesForSetVar_.begin(); it != distFactoriesForSetVar_.end(); ++it)
  {
    // get sample
    Description variablesNames(it->first);
    Indices indices;
    for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i)
    {
      bool varFound = false;
      for (UnsignedInteger j = 0; j < designOfExperiment_.getSample().getDescription().getSize(); ++j)
      {
        if (designOfExperiment_.getSample().getDescription()[j] == variablesNames[i])
        {
          indices.add(j);
          varFound = true;
          break;
        }
      }
      if (!varFound)
        throw InvalidArgumentException(HERE) << "The variable "  << variablesNames[i] << " is not a variable of the model " << designOfExperiment_.getSample().getDescription();
    }

    const Sample sample(designOfExperiment_.getSample().getMarginal(indices));

    Sample splitSample(sample);
    if (sample.getSize() > sizeKendall)
      Sample otherSample = splitSample.split(sizeKendall);

    // CopulaInferenceSetResult
    CopulaInferenceSetResult inferenceSetResult;
    inferenceSetResult.setOfVariablesNames_ = it->first;
    inferenceSetResult.errorMessages_ = Description(it->second.getSize());

    CombinatorialGeneratorImplementation::IndicesCollection pairs(Combinations(2, it->first.getSize()).generate());

    // for each copula:
    for (UnsignedInteger i = 0; i < it->second.getSize(); ++i)
    {
      try
      {
        // build distribution
        const Distribution distribution(it->second[i].build(sample));
        inferenceSetResult.testedDistributions_.add(distribution);

        // get Kendall plot data
        Description description(2);
        Collection<Sample> kendallPlotDataCollection;
        for (UnsignedInteger j = 0; j < pairs.getSize(); ++j)
        {
          const Graph graph = VisualTest::DrawKendallPlot(splitSample.getMarginal(pairs[j]), distribution.getMarginal(pairs[j]));
          Sample kendallPlotData(graph.getDrawable(1).getData());
          description[0] = sample.getDescription()[pairs[j][0]] + " - " + sample.getDescription()[pairs[j][1]];
          kendallPlotData.setDescription(description);
          kendallPlotDataCollection.add(kendallPlotData);
        }
        inferenceSetResult.kendallPlotData_.add(kendallPlotDataCollection);
      }
      catch (std::exception & ex)
      {
        String str = it->second[i].getImplementation()->getClassName();
        const String distributionName = str.substr(0, str.find("Copula"));
        const String message = OSS() << "Error when building the "
                                << distributionName
                                << " copula with the sample of the variables "
                                << sample.getDescription()
                                << ". "
                                << ex.what()
                                << "\n";
        // set fittingTestResult
        inferenceSetResult.testedDistributions_.add(DistributionDictionary::BuildCopulaFactory(distributionName).build());
        Collection<Sample> kendallPlotDataCollection;
        inferenceSetResult.kendallPlotData_.add(kendallPlotDataCollection);
        inferenceSetResult.errorMessages_[i] = message;
      }
    }
    // set result_
    result_.copulaInferenceSetResultCollection_.add(inferenceSetResult);
    result_.designOfExperiment_.setInputSample(designOfExperiment_.getInputSample());
    result_.designOfExperiment_.setOutputSample(designOfExperiment_.getOutputSample());
  }
}


CopulaInferenceResult CopulaInferenceAnalysis::getResult() const
{
  return result_;
}


String CopulaInferenceAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.CopulaInferenceAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

  std::map<Description, DistributionFactoryCollection>::const_iterator it;
  for (it = distFactoriesForSetVar_.begin(); it != distFactoriesForSetVar_.end(); ++it)
  {
    // variables list
    oss << "variablesSet = " << Parameters::GetOTDescriptionStr(it->first) << "\n";

    // factories list
    oss << "factories = [";
    for (UnsignedInteger j = 0; j < it->second.getSize(); ++j)
    {
      oss << "ot." << it->second[j].getImplementation()->getClassName() << "()";
      if (j < it->second.getSize() - 1)
        oss << ", ";
    }
    oss << "]\n";
    oss << getName() << ".setDistributionsFactories(variablesSet, factories)\n";
  }

  return oss;
}


bool CopulaInferenceAnalysis::hasValidResult() const
{
  return result_.getCopulaInferenceSetResultCollection().getSize() != 0;
}


String CopulaInferenceAnalysis::__repr__() const
{
  OSS oss;
  oss << DesignOfExperimentAnalysis::__repr__();

  std::map<Description, DistributionFactoryCollection>::const_iterator it;
  for (it = distFactoriesForSetVar_.begin(); it != distFactoriesForSetVar_.end(); ++it)
  {
    oss << " setOfVariables " << it->first
        << " distributionFactories=" << it->second;
  }

  return oss;
}


/* Method save() stores the object through the StorageManager */
void CopulaInferenceAnalysis::save(Advocate& adv) const
{
  DesignOfExperimentAnalysis::save(adv);

  PersistentCollection<Description> listSets;

  // TODO with new version of OT (where DistributionFactory can be saved)
//   DistributionFactoryCollectionCollection collection;
  PersistentCollection<Description> collection;

  std::map<Description, DistributionFactoryCollection>::const_iterator it;
  for (it = distFactoriesForSetVar_.begin(); it != distFactoriesForSetVar_.end(); ++it)
  {
    listSets.add(it->first);
    Description listFactoriesNames(it->second.getSize());
    for (UnsignedInteger j = 0; j < it->second.getSize(); ++j)
    {
      String str = it->second[j].getImplementation()->getClassName();
      listFactoriesNames[j] = str.substr(0, str.find("CopulaFactory"));
    }
    collection.add(listFactoriesNames);
  }
  adv.saveAttribute("listSetsOfVariables_", listSets);
  adv.saveAttribute("distFactoriesCollection_", collection);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void CopulaInferenceAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  PersistentCollection<Description> listSets;
  adv.loadAttribute("listSetsOfVariables_", listSets);
  PersistentCollection<Description> collection;
  adv.loadAttribute("distFactoriesCollection_", collection);
  adv.loadAttribute("result_", result_);

  for (UnsignedInteger i = 0; i < listSets.getSize(); ++i)
  {
    DistributionFactoryCollection factoryCollection;
    for (UnsignedInteger j = 0; j < collection[i].getSize(); ++j)
      factoryCollection.add(DistributionDictionary::BuildCopulaFactory(collection[i][j]));
    distFactoriesForSetVar_[listSets[i]] = factoryCollection;
  }
}
}
