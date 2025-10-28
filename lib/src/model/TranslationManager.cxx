//                                               -*- C++ -*-
/**
 *  @brief QObject to translate words
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
#include "persalys/TranslationManager.hxx"

#include "persalys/DistributionDictionary.hxx"

namespace PERSALYS
{

TranslationManager::bimap_type TranslationManager::DistributionsNames_;
TranslationManager::bimap_type TranslationManager::CopulasNames_;
TranslationManager::bimap_type TranslationManager::DistributionsParametersNames_;
TranslationManager::bimap_type TranslationManager::ParametersNames_;
TranslationManager::bimap_type TranslationManager::ErrorMessages_;


void TranslationManager::InitializeDistributionsNames()
{
  // list of all the available distributions
  DistributionsNames_.insert("Arcsine", tr("Arcsine"));
  DistributionsNames_.insert("Bernoulli", "Bernoulli");
  DistributionsNames_.insert("Beta", tr("Beta"));
  DistributionsNames_.insert("Binomial", "Binomial");
  DistributionsNames_.insert("ChiSquare", tr("ChiSquare"));
  DistributionsNames_.insert("Exponential", tr("Exponential"));
  DistributionsNames_.insert("Gamma", tr("Gamma"));
  DistributionsNames_.insert("Geometric", tr("Geometric"));
  DistributionsNames_.insert("Gumbel", "Gumbel");
  DistributionsNames_.insert("Hypergeometric", tr("Hypergeometric"));
  DistributionsNames_.insert("InverseNormal", tr("InverseNormal"));
  DistributionsNames_.insert("Laplace", "Laplace");
  DistributionsNames_.insert("Logistic", tr("Logistic"));
  DistributionsNames_.insert("LogNormal", tr("LogNormal"));
  DistributionsNames_.insert("LogUniform", tr("LogUniform"));
  DistributionsNames_.insert("Normal", tr("Normal"));
  DistributionsNames_.insert("Pareto", "Pareto");
  DistributionsNames_.insert("Poisson", "Poisson");
  DistributionsNames_.insert("Polya", "Polya");
  DistributionsNames_.insert("Rayleigh", "Rayleigh");
  DistributionsNames_.insert("Skellam", "Skellam");
  DistributionsNames_.insert("SmoothedUniform", tr("Smoothed uniform"));
  DistributionsNames_.insert("Student", "Student");
  DistributionsNames_.insert("Trapezoidal", tr("Trapezoidal"));
  DistributionsNames_.insert("Triangular", tr("Triangular"));
  DistributionsNames_.insert("Uniform", tr("Uniform"));
  DistributionsNames_.insert("WeibullMax", "WeibullMax");
  DistributionsNames_.insert("WeibullMin", "WeibullMin");
  DistributionsNames_.insert("ZipfMandelbrot", "Zipf-Mandelbrot");
}


void TranslationManager::InitializeCopulasNames()
{
  // list of all the available copulas
  CopulasNames_.insert("AliMikhailHaq", "Ali-Mikhail-Haq");
  CopulasNames_.insert("Clayton", "Clayton");
  CopulasNames_.insert("FarlieGumbelMorgenstern", "Farlie-Gumbel-\nMorgenstern");
  CopulasNames_.insert("Frank", "Frank");
  CopulasNames_.insert("Gumbel", "Gumbel");
  CopulasNames_.insert("Independent", "Independent");
  CopulasNames_.insert("Normal", tr("Normal"));
  CopulasNames_.insert("Student", tr("Student"));
}


void TranslationManager::InitializeDistributionsParametersNames()
{
  DistributionsParametersNames_.insert("nu", "ν");
  DistributionsParametersNames_.insert("alpha", "α");
  DistributionsParametersNames_.insert("beta", "β");
  DistributionsParametersNames_.insert("theta", "θ");
  DistributionsParametersNames_.insert("gamma", "γ");
  DistributionsParametersNames_.insert("sigma", "σ");
  DistributionsParametersNames_.insert("lambda", "λ");
  DistributionsParametersNames_.insert("mu", "μ");
  DistributionsParametersNames_.insert("sigmaOverMu", "σ/μ");
  DistributionsParametersNames_.insert("muLog", "μ_Log");
  DistributionsParametersNames_.insert("sigmaLog", "σ_Log");
  DistributionsParametersNames_.insert("aLog", "a_Log");
  DistributionsParametersNames_.insert("bLog", "b_Log");
  DistributionsParametersNames_.insert("lowerBound", tr("lower bound"));
  DistributionsParametersNames_.insert("upperBound", tr("upper bound"));
}


void TranslationManager::InitializeParametersNames()
{
  ParametersNames_.insert("Seed", tr("Seed"));
  ParametersNames_.insert("Algorithm", tr("Algorithm"));
  ParametersNames_.insert("Taylor expansion moments", tr("Taylor expansion moments"));
  ParametersNames_.insert("Confidence level", tr("Confidence level"));
  ParametersNames_.insert("Maximum coefficient of variation", tr("Maximum coefficient of variation"));
  ParametersNames_.insert("Maximum elapsed time", tr("Maximum elapsed time"));
  ParametersNames_.insert("Maximum calls", tr("Maximum calls"));
  ParametersNames_.insert("Block size", tr("Block size"));
  ParametersNames_.insert("Design name", tr("Design name"));
  ParametersNames_.insert("Monte Carlo", tr("Monte Carlo"));
  ParametersNames_.insert("Quasi-Monte Carlo", tr("Quasi-Monte Carlo"));
  ParametersNames_.insert("Covariance model", tr("Covariance model"));
  ParametersNames_.insert("nu", tr("nu"));
  ParametersNames_.insert("p", tr("p"));
  ParametersNames_.insert("Parameters optimization", tr("Parameters optimization"));
  ParametersNames_.insert("Scale", tr("Scale"));
  ParametersNames_.insert("Amplitude", tr("Amplitude"));
  ParametersNames_.insert("Trend basis", tr("Trend basis"));
  ParametersNames_.insert("Leave-one-out validation", tr("Leave-one-out validation"));
  ParametersNames_.insert("Analytical validation", tr("Analytical validation"));
  ParametersNames_.insert("Test sample validation", tr("Test sample validation"));
  ParametersNames_.insert("KFold validation", tr("KFold validation"));
  ParametersNames_.insert("Kriging", tr("Kriging"));
  ParametersNames_.insert("yes", tr("yes"));
  ParametersNames_.insert("no", tr("no"));
  ParametersNames_.insert("Constant", tr("Constant"));
  ParametersNames_.insert("Linear", tr("Linear"));
  ParametersNames_.insert("Quadratic", tr("Quadratic"));
  ParametersNames_.insert("Chaos degree", tr("Chaos degree"));
  ParametersNames_.insert("Sparse", tr("Sparse"));
  ParametersNames_.insert("Functional chaos", tr("Functional chaos"));
  ParametersNames_.insert("Sobol", tr("Sobol"));
  ParametersNames_.insert("Sample size", tr("Sample size"));
  ParametersNames_.insert("Point", tr("Point"));
  ParametersNames_.insert("Outputs of interest", tr("Outputs of interest"));
  ParametersNames_.insert("Output of interest", tr("Output of interest"));
  ParametersNames_.insert("Standardized Regression Coefficients", tr("Standardized Regression Coefficients"));
  ParametersNames_.insert("Optimization algorithm", tr("Optimization algorithm"));
  ParametersNames_.insert("Optimization type", tr("Optimization type"));
  ParametersNames_.insert("Fixed inputs", tr("Fixed inputs"));
  ParametersNames_.insert("Solver", tr("Solver"));
  ParametersNames_.insert("Analysis", tr("Analysis"));
  ParametersNames_.insert("Starting point", tr("Starting point"));
  ParametersNames_.insert("Physical starting point", tr("Physical starting point"));
  ParametersNames_.insert("Maximum iterations number", tr("Maximum iterations number"));
  ParametersNames_.insert("Maximum absolute error", tr("Maximum absolute error"));
  ParametersNames_.insert("Maximum relative error", tr("Maximum relative error"));
  ParametersNames_.insert("Maximum residual error", tr("Maximum residual error"));
  ParametersNames_.insert("Maximum constraint error", tr("Maximum constraint error"));
  ParametersNames_.insert("FORM", tr("FORM"));
  ParametersNames_.insert("Design point (standard space)", tr("Design point (standard space)"));
  ParametersNames_.insert("FORM - Importance sampling", tr("FORM - Importance sampling"));
  ParametersNames_.insert("Importance sampling", tr("Importance sampling"));
  ParametersNames_.insert("Method", tr("Method"));
  ParametersNames_.insert("Level", tr("Level"));
  ParametersNames_.insert("Kolmogorov-Smirnov", tr("Kolmogorov-Smirnov"));
  ParametersNames_.insert("Lilliefors", tr("Lilliefors"));
  ParametersNames_.insert("Number of folds", tr("Number of folds"));
  ParametersNames_.insert("Percentage", tr("Percentage"));
  ParametersNames_.insert("Maximum confidence interval length", tr("Maximum confidence interval length"));
  ParametersNames_.insert("Replication size", tr("Replication size"));
  ParametersNames_.insert("Bounds", tr("Bounds"));
  ParametersNames_.insert("Design type", tr("Design type"));
  ParametersNames_.insert("Grid", tr("Grid"));
  ParametersNames_.insert("File", tr("File"));
  ParametersNames_.insert("Imported", tr("Imported"));
  ParametersNames_.insert("Analytical", tr("Analytical"));
  ParametersNames_.insert("Test sample", tr("Test sample"));
  ParametersNames_.insert("K-Fold", tr("K-Fold"));
  ParametersNames_.insert("Leave-one-out", tr("Leave-one-out"));
  ParametersNames_.insert("Physical model", tr("Physical model"));
  ParametersNames_.insert("Deterministic inputs", tr("Deterministic inputs"));
  ParametersNames_.insert("Stochastic inputs", tr("Stochastic inputs"));
  ParametersNames_.insert("Inputs", tr("Inputs"));
  ParametersNames_.insert("Outputs", tr("Outputs"));
  ParametersNames_.insert("Limit state", tr("Limit state"));
  ParametersNames_.insert("Name", tr("Name"));
  ParametersNames_.insert("Description", tr("Description"));
  ParametersNames_.insert("Value", tr("Value"));
  ParametersNames_.insert("Distribution", tr("Distribution"));
  ParametersNames_.insert("Dependence", tr("Dependence"));
  ParametersNames_.insert("Group of variables", tr("Group of variables"));
  ParametersNames_.insert("Copula", tr("Copula"));
  ParametersNames_.insert("Formula", tr("Formula"));
  ParametersNames_.insert("File name", tr("File name"));
  ParametersNames_.insert("Number of trajectories", tr("Number of trajectories"));
  ParametersNames_.insert("Columns", tr("Columns"));
  ParametersNames_.insert("Observed variables", tr("Observed variables"));
  ParametersNames_.insert("Variables to calibrate", tr("Variables to calibrate"));
  ParametersNames_.insert("θ prior distribution", tr("θ prior distribution"));
  ParametersNames_.insert("θ prior\nSpearman correlation", tr("θ prior\nSpearman correlation"));
  ParametersNames_.insert("Error\ncovariance", tr("Error\ncovariance"));
  ParametersNames_.insert("θ prior", tr("θ prior"));
  ParametersNames_.insert("Confidence interval length", tr("Confidence interval length"));
  ParametersNames_.insert("Bootstrap resampling size", tr("Bootstrap resampling size"));
  ParametersNames_.insert("Maximum number of calls", tr("Maximum number of calls"));
  ParametersNames_.insert("Number of generations", tr("Number of generations"));
  ParametersNames_.insert("Initial population size", tr("Initial population size"));
  ParametersNames_.insert("Initial population size", tr("Initial population size"));
  ParametersNames_.insert("Objectives", tr("Objectives"));
  ParametersNames_.insert("Constraints", tr("Constraints"));
  ParametersNames_.insert("Minimization", tr("Minimization"));
  ParametersNames_.insert("Maximization", tr("Maximization"));
  ParametersNames_.insert("Optimization", tr("Optimization"));
  ParametersNames_.insert("Multi-objective optimization", tr("Multi-objective optimization"));
}


void TranslationManager::InitializeErrorMessages()
{
  ErrorMessages_.insert("The model must contain at least two variables.", tr("The model must contain at least two variables."));
  ErrorMessages_.insert("The sample must be not empty.", tr("The sample must be not empty."));
  ErrorMessages_.insert("The sample contains invalid values.", tr("The sample contains invalid values."));
  ErrorMessages_.insert("The physical model must have inputs.", tr("The physical model must have inputs."));
  ErrorMessages_.insert("The design of experiments must contain data for input and output variables.", tr("The design of experiments must contain data for input and output variables."));
  ErrorMessages_.insert("The physical model must have at least two inputs.", tr("The physical model must have at least two inputs."));
  ErrorMessages_.insert("The physical model must have at least two outputs.", tr("The physical model must have at least two outputs."));
  ErrorMessages_.insert("The physical model must have inputs and at least a selected output.", tr("The physical model must have inputs and at least a selected output."));
  ErrorMessages_.insert("The physical model must have stochastic inputs.", tr("The physical model must have stochastic inputs."));
  ErrorMessages_.insert("The model must have an independent copula to compute a sensitivity analysis but here inputs are dependent.", tr("The model must have an independent copula to compute a sensitivity analysis but here inputs are dependent."));
  ErrorMessages_.insert("The limit state is not valid.", tr("The limit state is not valid."));
  //GridMeshModel::setParameters
  ErrorMessages_.insert("The lower bounds must be less than the upper bounds", tr("The lower bounds must be less than the upper bounds"));
  ErrorMessages_.insert("The mesh must contain at least two node", tr("The mesh must contain at least two node"));
}


QString TranslationManager::GetTranslatedDistributionName(const std::string& name)
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  return DistributionsNames_.translate(name.c_str());
}


QString TranslationManager::GetTranslatedCopulaName(const std::string& name)
{
  if (CopulasNames_.empty())
    InitializeCopulasNames();

  return CopulasNames_.translate(name.c_str());
}


QString TranslationManager::GetTranslatedDistributionParameterName(const std::string& name)
{
  if (DistributionsParametersNames_.empty())
    InitializeDistributionsParametersNames();

  QString paramName = name.c_str();
  if (paramName == "mean_0")
    paramName = "mu";
  else if (paramName == "standard_deviation_0")
    paramName = "sigma";

  return DistributionsParametersNames_.translate(paramName);
}


QString TranslationManager::GetTranslatedParameterName(const std::string& name)
{
  if (ParametersNames_.empty())
    InitializeParametersNames();

  return ParametersNames_.translate(name.c_str());
}


std::string TranslationManager::GetDistributionName(const QString& name)
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  return DistributionsNames_.untranslate(name).toStdString();
}


std::string TranslationManager::GetCopulaName(const QString& name)
{
  if (CopulasNames_.empty())
    InitializeCopulasNames();

  return CopulasNames_.untranslate(name).toStdString();
}


/* Get the list of the available distributions */
QStringList TranslationManager::GetTranslatedContinuousDistributions()
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  std::vector<std::string> continuousDist(DistributionDictionary::ContinuousDistributions_);
  QStringList distributions;
  for (int i = 0; i < (int)continuousDist.size(); ++i)
    distributions << GetTranslatedDistributionName(continuousDist[i]);
  return distributions;
}


/* Get the list of the available distributions */
QStringList TranslationManager::GetTranslatedDiscreteDistributions()
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  std::vector<std::string> discreteDist(DistributionDictionary::DiscreteDistributions_);
  QStringList distributions;
  for (int i = 0; i < (int)discreteDist.size(); ++i)
    distributions << GetTranslatedDistributionName(discreteDist[i]);
  return distributions;
}


/* Get the list of the available copulas */
QStringList TranslationManager::GetAvailableCopulas()
{
  if (CopulasNames_.empty())
    InitializeCopulasNames();

  QStringList copulas;
  for (bimap_type::map_type::const_iterator left_iter = CopulasNames_.left.begin(), iend = CopulasNames_.left.end(); left_iter != iend; ++left_iter)
    copulas << left_iter->second;
  return copulas;
}


QString TranslationManager::GetTranslatedErrorMessage(const std::string& message)
{
  if (ErrorMessages_.empty())
    InitializeErrorMessages();

  return ErrorMessages_.translate(message.c_str());
}
}
