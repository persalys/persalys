//                                               -*- C++ -*-
/**
 *  @brief Inference analysis
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
#include "otgui/InferenceAnalysis.hxx"

#include "otgui/DistributionDictionary.hxx"

#include <openturns/FittingTest.hxx>
#include <openturns/NormalFactory.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(InferenceAnalysis);

static Factory<InferenceAnalysis> RegisteredFactory;
static Factory<PersistentCollection<Description> > RegisteredFactory_CollDesc;

/* Default constructor */
InferenceAnalysis::InferenceAnalysis()
  : DesignOfExperimentAnalysis()
  , distFactoriesForEachInterestVar_()
  , level_(0.95)
{
}


/* Constructor with parameters */
InferenceAnalysis::InferenceAnalysis(const String & name, const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , distFactoriesForEachInterestVar_()
  , level_(0.95)
{
  // by default we test the Normal distribution for all the variables of the DOE
  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    DistributionFactoryCollection collectionVariable_i;
    collectionVariable_i.add(NormalFactory());
    distFactoriesForEachInterestVar_[getInterestVariables()[i]] = collectionVariable_i;
  }
}


/* Virtual constructor */
InferenceAnalysis* InferenceAnalysis::clone() const
{
  return new InferenceAnalysis(*this);
}


InferenceAnalysis::DistributionFactoryCollection InferenceAnalysis::getDistributionsFactories(const String& variableName) const
{
  if (!designOfExperiment_.getSample().getDescription().contains(variableName))
    throw InvalidArgumentException(HERE) << "Error: the given variable name does not match a variable of the model";

  std::map<String, DistributionFactoryCollection>::const_iterator it(distFactoriesForEachInterestVar_.find(variableName));
  if (it == distFactoriesForEachInterestVar_.end())
    throw InvalidArgumentException(HERE) << "Error: no distribution factories set for the variable " << variableName;

  return it->second;
}


void InferenceAnalysis::setDistributionsFactories(const String& variableName, const DistributionFactoryCollection& distributionsFactories)
{
  for (UnsignedInteger i=0; i<distributionsFactories.getSize(); ++i)
    if (!distributionsFactories.getSize())
      throw InvalidArgumentException(HERE) << "Error: the list of distribution factories must have a dimension superior to 0";

  if (!designOfExperiment_.getSample().getDescription().contains(variableName))
    throw InvalidArgumentException(HERE) << "Error: the given variable name does not match a variable of the model";

  Description interestVariables(getInterestVariables());
  if (!interestVariables.contains(variableName))
  {
    interestVariables.add(variableName);
    setInterestVariables(interestVariables);
  }
  distFactoriesForEachInterestVar_[variableName] = distributionsFactories;
}


double InferenceAnalysis::getLevel() const
{
  return level_;
}


void InferenceAnalysis::setLevel(const double level)
{
  // check value
  if ((level <= 0.0) || (level >= 1.0))
    throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  level_ = level;
}


void InferenceAnalysis::setInterestVariables(const Description& variablesNames)
{
  DesignOfExperimentAnalysis::setInterestVariables(variablesNames);

  std::map<String, DistributionFactoryCollection>::iterator it;
  for (it=distFactoriesForEachInterestVar_.begin(); it!=distFactoriesForEachInterestVar_.end(); ++it)
    if (!variablesNames.contains(it->first))
      distFactoriesForEachInterestVar_.erase(it->first);
}


void InferenceAnalysis::run()
{
  // clear result
  result_ = InferenceResult();

  // check
  if (!getInterestVariables().getSize())
    throw InvalidDimensionException(HERE) << "The number of variables to analyse must be superior to 0";

  std::map<String, DistributionFactoryCollection>::iterator it;
  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    it = distFactoriesForEachInterestVar_.find(getInterestVariables()[i]);
    if (it == distFactoriesForEachInterestVar_.end())
      throw InvalidArgumentException(HERE) << "Error: no distribution factories set for the variable " << getInterestVariables()[i];
  }

  // get marginals
  Indices indices;
  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    bool outputFound = false;
    for (UnsignedInteger j=0; j<designOfExperiment_.getSample().getDescription().getSize(); ++j)
    {
      if (designOfExperiment_.getSample().getDescription()[j] == getInterestVariables()[i])
      {
        indices.add(j);
        outputFound = true;
        break;
      }
    }
    if (!outputFound)
      throw InvalidArgumentException(HERE) << "The variable to analyse "  << getInterestVariables()[i] <<" is not a variable of the model " << designOfExperiment_.getSample().getDescription();
  }

  const NumericalSample sample(designOfExperiment_.getSample().getMarginal(indices));

  // inference
  for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
  {
    FittingTestResult fittingTestResult;
    fittingTestResult.variableName_ = sample.getDescription()[i];
    fittingTestResult.values_ = sample.getMarginal(i);

    for (UnsignedInteger j=0; j<distFactoriesForEachInterestVar_[sample.getDescription()[i]].getSize(); ++j)
    {
      try
      {
        Distribution distribution(distFactoriesForEachInterestVar_[sample.getDescription()[i]][j].build(sample.getMarginal(i)));

        // Kolmogorov test
        TestResult testResult(FittingTest::Kolmogorov(sample.getMarginal(i), distribution, level_, distribution.getParameterDimension()));

        // set fittingTestResult
        fittingTestResult.testedDistributions_.add(distribution);
        fittingTestResult.kolmogorovTestResults_.add(testResult);
      }
      catch (std::exception & ex)
      {
        String str = distFactoriesForEachInterestVar_[sample.getDescription()[i]][j].getImplementation()->getClassName();
        throw InvalidValueException(HERE) << "Error when building the "
                                          << str.substr(0, str.find("Factory"))
                                          << " distribution with the sample of the variable "
                                          << sample.getDescription()[i]
                                          << ". "
                                          << ex.what();
      }
    }
    result_.fittingTestResultCollection_.add(fittingTestResult);
  }
  notify("analysisFinished");
}


InferenceResult InferenceAnalysis::getResult() const
{
  return result_;
}


String InferenceAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.InferenceAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";
  oss << "interestVariables = [";
  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    oss << "'" << getInterestVariables()[i] << "'";
    if (i < getInterestVariables().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";

  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    oss << "factories = [";
    DistributionFactoryCollection collection(getDistributionsFactories(getInterestVariables()[i]));
    for (UnsignedInteger j=0; j<collection.getSize(); ++j)
    {
      oss << "ot." << collection[j].getImplementation()->getClassName() << "()";
      if (j < collection.getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << getName() << ".setDistributionsFactories('" << getInterestVariables()[i] << "', factories)\n";
  }
  oss << getName() << ".setLevel(" << getLevel() << ")\n";

  return oss;
}


bool InferenceAnalysis::analysisLaunched() const
{
  return result_.getFittingTestResultCollection().getSize() != 0;
}


String InferenceAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " designOfExperiment=class=" << getDesignOfExperiment().GetClassName() << " name=" << getDesignOfExperiment().getName()
      << " level=" << getLevel();
      for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
      {
        oss << " interestVariable " << getInterestVariables()[i]
            << " distributionFactories=" << getDistributionsFactories(getInterestVariables()[i]);
      }
  return oss;
}


/* Method save() stores the object through the StorageManager */
void InferenceAnalysis::save(Advocate& adv) const
{
  DesignOfExperimentAnalysis::save(adv);

// TODO with new version of OT (where DistributionFactory can be saved)
//   DistributionFactoryCollectionCollection collection;
  PersistentCollection<Description> collection;
  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    DistributionFactoryCollection factoryCollection(distFactoriesForEachInterestVar_.find(getInterestVariables()[i])->second);
    Description listFactoriesNames(factoryCollection.getSize());
    for (UnsignedInteger j=0; j<factoryCollection.getSize(); ++j)
    {
      String str = factoryCollection[j].getImplementation()->getClassName();
      listFactoriesNames[j] = str.substr(0, str.find("Factory"));
    }
    collection.add(listFactoriesNames);
  }
  adv.saveAttribute("distFactoriesCollection_", collection);
  adv.saveAttribute("level_", level_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void InferenceAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  PersistentCollection<Description> collection;
  adv.loadAttribute("distFactoriesCollection_", collection);
  adv.loadAttribute("level_", level_);
  adv.loadAttribute("result_", result_);

  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    DistributionFactoryCollection factoryCollection;
    for (UnsignedInteger j=0; j<collection[i].getSize(); ++j)
      factoryCollection.add(DistributionDictionary::BuildDistributionFactory(collection[i][j]));
    distFactoriesForEachInterestVar_[getInterestVariables()[i]] = factoryCollection;
  }
}
}