//                                               -*- C++ -*-
/**
 *  @brief Inference analysis
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
#include "persalys/CopulaInferenceAnalysis.hxx"

#include "persalys/CopulaInferenceSetResult.hxx"

#include <openturns/DistributionFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/Combinations.hxx>
#include <openturns/VisualTest.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/FittingTest.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CopulaInferenceAnalysis)

static Factory<CopulaInferenceAnalysis> Factory_CopulaInferenceAnalysis;
static Factory<PersistentCollection<Description> > Factory_CollectionDescription;
static Factory<PersistentCollection<DistributionFactory > > Factory_CollectionFactories;
static Factory<PersistentCollection<PersistentCollection<DistributionFactory > > > Factory_CollectionCollectionFactories;

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
  // find groups of dependent variables
  Collection<Description> groups = buildDefaultVariablesGroups();

  // by default we test the Normal copula
  DistributionFactoryCollection defaultCopula(1, NormalCopulaFactory());
  for (UnsignedInteger i = 0; i < groups.getSize(); ++i)
    setDistributionsFactories(groups[i], defaultCopula);
}


/* Constructor with parameters */
CopulaInferenceAnalysis::CopulaInferenceAnalysis(const String &name, const DesignOfExperiment &designOfExperiment, const Collection<Description> &groups)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , distFactoriesForSetVar_()
{
  // by default we test the Normal copula
  DistributionFactoryCollection defaultCopula(1, NormalCopulaFactory());
  for (UnsignedInteger i = 0; i < groups.getSize(); ++i)
    setDistributionsFactories(groups[i], defaultCopula);
}


Collection<Description> CopulaInferenceAnalysis::buildDefaultVariablesGroups()
{
  // Spearman correlation matrix
  CorrelationMatrix C(designOfExperiment_.getSample().computeSpearmanCorrelation());

  // consider only significantly non-zero correlations
  const double alpha = 0.05;
  const double epsilon = Normal().computeQuantile(1 - alpha)[0] / std::sqrt(designOfExperiment_.getSample().getSize() - 1);

  for (UnsignedInteger j = 0; j < designOfExperiment_.getSample().getDimension(); ++j)
    for (UnsignedInteger i = 0; i < j; ++i)
      C(i, j) = (std::abs(C(i, j)) > epsilon) ? 1.0 : 0.0;

  // find groups of dependent variables
  Collection<Description> groups;
  Collection<Indices> components = ConnectedComponents(C);

  for (UnsignedInteger i = 0; i < components.getSize(); ++i)
  {
    if (components[i].getSize() > 1)
    {
      Description variables(designOfExperiment_.getSample().getMarginal(components[i]).getDescription());
      groups.add(variables);
    }
  }
  return groups;
}


// find connected components of a graph defined from its adjacency matrix
Indices CopulaInferenceAnalysis::FindNeighbours(const UnsignedInteger head, const CorrelationMatrix &matrix, Indices &to_visit, Indices &visited)
{
  visited[head] = 1;
  to_visit.erase(std::remove(to_visit.begin(), to_visit.end(), head), to_visit.end());
  Indices current_component(1, head);
  for (UnsignedInteger i = 0; i < to_visit.getSize(); ++i)
  {
    // If i is connected to head and has not yet been visited
    if (matrix(head, to_visit[i]) > 0)
    {
      // Add i to the current component
      Indices component(FindNeighbours(to_visit[i], matrix, to_visit, visited));
      current_component.add(component);
    }
  }
  return current_component;
}


Collection<Indices> CopulaInferenceAnalysis::ConnectedComponents(const CorrelationMatrix &matrix)
{
  UnsignedInteger N = matrix.getDimension();
  Indices to_visit(N);
  to_visit.fill();
  Indices visited(N);
  Collection<Indices> all_components;
  for (UnsignedInteger head = 0; head < N; ++head)
  {
    if (visited[head] == 0)
    {
      Indices component(FindNeighbours(head, matrix, to_visit, visited));
      std::sort(component.begin(), component.end());
      all_components.add(component);
    }
  }
  return all_components;
}


/* Virtual constructor */
CopulaInferenceAnalysis* CopulaInferenceAnalysis::clone() const
{
  return new CopulaInferenceAnalysis(*this);
}


CopulaInferenceAnalysis::DistributionFactoryCollection CopulaInferenceAnalysis::getDistributionsFactories(const Description& variablesNames) const
{
  const Description setOfVariables(getSortedVariablesNames(variablesNames));

  std::map<Description, DistributionFactoryCollection>::const_iterator it(distFactoriesForSetVar_.find(setOfVariables));
  if (it == distFactoriesForSetVar_.end())
    throw InvalidArgumentException(HERE) << "Error: no distribution factories set for the set of variables " << variablesNames;

  return it->second;
}


Collection<Description> CopulaInferenceAnalysis::getVariablesGroups()
{
  Collection<Description> groups;
  std::map<Description, DistributionFactoryCollection>::iterator it;
  for (it = distFactoriesForSetVar_.begin(); it != distFactoriesForSetVar_.end(); ++it)
  {
    groups.add(it->first);
  }
  return groups;
}


void CopulaInferenceAnalysis::setDistributionsFactories(const Description& variablesNames, const DistributionFactoryCollection& factories)
{
  if (variablesNames.getSize() < 2)
    throw InvalidArgumentException(HERE) << "Error: the dependence inference is performed with at least 2 variables";

  for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
    if (factories[i].getImplementation()->getClassName().find("Copula") == std::string::npos)
      throw InvalidArgumentException(HERE) << "Error: the dependence inference is performed with copulae.";

  const Description setOfVariables(getSortedVariablesNames(variablesNames));

  // cancel inference for the given set of variables
  if (!factories.getSize())
    distFactoriesForSetVar_.erase(setOfVariables);
  // test all the given factories
  else
    distFactoriesForSetVar_[setOfVariables] = factories;
}


Description CopulaInferenceAnalysis::getSortedVariablesNames(const Description& variablesNames) const
{
  const Description allVars(designOfExperiment_.getSample().getDescription());
  for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i)
    if (!allVars.contains(variablesNames[i]))
      throw InvalidArgumentException(HERE) << "Error: the variable name " << variablesNames[i] << " does not match a variable of the model";

  // keep same order as in the sample description
  Indices indices;
  for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(allVars.begin(), allVars.end(), variablesNames[i]);
    indices.add(it - allVars.begin());
  }
  std::sort(indices.begin(), indices.end());

  return designOfExperiment_.getSample().getMarginal(indices).getDescription();
}


void CopulaInferenceAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = CopulaInferenceResult();
}


void CopulaInferenceAnalysis::launch()
{
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

    Sample sample(designOfExperiment_.getSample().getMarginal(indices));
    sample = (sample.rank() + 0.5) / sample.getSize();

    Sample splitSample(sample);

    // CopulaInferenceSetResult
    CopulaInferenceSetResult inferenceSetResult;
    inferenceSetResult.setOfVariablesNames_ = it->first;
    inferenceSetResult.errorMessages_ = Description(it->second.getSize());

    IndicesCollection pairs(Combinations(2, it->first.getSize()).generate());

    // for each copula:
    for (UnsignedInteger i = 0; i < it->second.getSize(); ++i)
    {
      try
      {
        // build distribution
        const Distribution distribution(it->second[i].build(sample));
        inferenceSetResult.testedDistributions_.add(distribution);

        // BIC test
        const Scalar bicResult = FittingTest::BIC(sample, distribution, distribution.getParameterDimension());
        inferenceSetResult.bicResults_.add(bicResult);

        // get Kendall plot data
        Description description(2);
        ProcessSample kendallPlotDataCollection(Mesh(Sample(splitSample.getSize(), 1)), 0, 1);
        for (UnsignedInteger j = 0; j < pairs.getSize(); ++j)
        {
          Indices ind(pairs.cbegin_at(j), pairs.cend_at(j));
          const Graph graph = VisualTest::DrawKendallPlot(splitSample.getMarginal(ind), distribution.getMarginal(ind));
          Sample kendallPlotData(graph.getDrawable(1).getData());
          description[0] = sample.getDescription()[ind[0]] + " - " + sample.getDescription()[ind[1]];
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
        inferenceSetResult.testedDistributions_.add(DistributionFactory::GetByName(distributionName+"Factory").build());
        ProcessSample kendallPlotDataCollection;
        inferenceSetResult.bicResults_.add(SpecFunc::MaxScalar);
        inferenceSetResult.kendallPlotData_.add(kendallPlotDataCollection);
        inferenceSetResult.errorMessages_[i] = message;
      }
    }
    // set result_
    result_.designOfExperiment_ = designOfExperiment_;
    result_.copulaInferenceSetResultCollection_.add(inferenceSetResult);
  }
}


CopulaInferenceResult CopulaInferenceAnalysis::getResult() const
{
  return result_;
}


String CopulaInferenceAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.CopulaInferenceAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

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


bool CopulaInferenceAnalysis::canBeLaunched(String &errorMessage) const
{
  const bool canBeLaunched = DesignOfExperimentAnalysis::canBeLaunched(errorMessage);
  if (!canBeLaunched)
    return false;
  if (designOfExperiment_.getSample().getDimension() < 2)
    errorMessage = "The model must contain at least two variables.";
  return errorMessage.empty();
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
  PersistentCollection<PersistentCollection<DistributionFactory > > listFactories;

  std::map<Description, DistributionFactoryCollection>::const_iterator it;
  for (it = distFactoriesForSetVar_.begin(); it != distFactoriesForSetVar_.end(); ++it)
  {
    listSets.add(it->first);
    listFactories.add(it->second);
  }
  adv.saveAttribute("listSetsOfVariables_", listSets);
  adv.saveAttribute("distFactoriesCollection_", listFactories);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void CopulaInferenceAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  PersistentCollection<Description> listSets;
  adv.loadAttribute("listSetsOfVariables_", listSets);
  PersistentCollection<PersistentCollection<DistributionFactory > > listFactories;
  adv.loadAttribute("distFactoriesCollection_", listFactories);
  adv.loadAttribute("result_", result_);

  for (UnsignedInteger i = 0; i < listSets.getSize(); ++i)
  {
    distFactoriesForSetVar_[listSets[i]] = listFactories[i];
  }
}
}
