//                                               -*- C++ -*-
/**
 *  @brief Class to define kriging
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
#include "persalys/KrigingAnalysis.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/GaussianProcessFitter.hxx>
#include <openturns/ConstantBasisFactory.hxx>
#include <openturns/SquaredExponential.hxx>
#include <openturns/AggregatedFunction.hxx>
#include <openturns/GeneralizedExponential.hxx>
#include <openturns/MaternModel.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(KrigingAnalysis)

static Factory<KrigingAnalysis> Factory_KrigingAnalysis;

/* Default constructor */
KrigingAnalysis::KrigingAnalysis()
  : MetaModelAnalysis()
  , basis_()
  , covarianceModel_()
  , result_()
  , optimizeParameters_(true)
{

}


/* Constructor with parameters */
KrigingAnalysis::KrigingAnalysis(const String& name, const DesignOfExperiment& designOfExperiment)
  : MetaModelAnalysis(name, designOfExperiment)
  , basis_()
  , covarianceModel_()
  , result_()
  , optimizeParameters_(true)
{
  const UnsignedInteger inputDimension = getEffectiveInputSample().getDimension();

  // basis
  setBasis(ConstantBasisFactory(inputDimension).build());
  // cov model
  setCovarianceModel(SquaredExponential(inputDimension));
}


/* Constructor with parameters */
KrigingAnalysis::KrigingAnalysis(const String& name, const Analysis& analysis)
  : MetaModelAnalysis(name, analysis)
  , basis_()
  , covarianceModel_()
  , result_()
  , optimizeParameters_(true)
{
  const UnsignedInteger inputDimension = getEffectiveInputSample().getDimension();

  // basis
  setBasis(ConstantBasisFactory(inputDimension).build());
  // cov model
  setCovarianceModel(SquaredExponential(inputDimension));
}


/* Virtual constructor */
KrigingAnalysis* KrigingAnalysis::clone() const
{
  return new KrigingAnalysis(*this);
}


Basis KrigingAnalysis::getBasis() const
{
  return basis_;
}


void KrigingAnalysis::setBasis(const Basis& basis)
{
  basis_ = basis;
}


CovarianceModel KrigingAnalysis::getCovarianceModel() const
{
  return covarianceModel_;
}


void KrigingAnalysis::setCovarianceModel(const CovarianceModel& model)
{
  if (model.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "The covariance model dimension must be equal to 1 ";

  covarianceModel_ = model;
}


bool KrigingAnalysis::getOptimizeParameters() const
{
  return optimizeParameters_;
}


void KrigingAnalysis::setOptimizeParameters(const bool optimize)
{
  optimizeParameters_ = optimize;
}


void KrigingAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = KrigingAnalysisResult();
  optimalCovarianceModel_ = CovarianceModel();
}


void KrigingAnalysis::launch()
{
  // get effective samples
  const Sample effectiveInputSample(getEffectiveInputSample());
  const Sample effectiveOutputSample(getEffectiveOutputSample());
  const UnsignedInteger inputSize = effectiveInputSample.getSize();
  const UnsignedInteger inputDimension = effectiveInputSample.getDimension();
  const UnsignedInteger outputDimension = effectiveOutputSample.getDimension();
  const Description inputVariables(effectiveInputSample.getDescription());
  const Description outputVariables(effectiveOutputSample.getDescription());

  // check
  if (inputSize * designOfExperiment_.getOutputSample().getSize() == 0)
    throw InvalidArgumentException(HERE) << "The design of experiments must contains not empty input AND output samples";
  if (inputSize != designOfExperiment_.getOutputSample().getSize())
    throw InvalidArgumentException(HERE) << "The input sample and the output sample must have the same size";
  if (getCovarianceModel().getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "The covariance model spatial dimension (" << getCovarianceModel().getInputDimension()
                                         << ") must be equal to the number of effective inputs (" << inputDimension << ")";
  if (getBasis().getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "The basis dimension (" << getBasis().getInputDimension()
                                         << ") must be equal to the number of effective inputs (" << inputDimension << ")";
  if (testSampleValidation() && inputSize * getTestSampleValidationPercentageOfPoints() / 100 < 3)
    throw InvalidArgumentException(HERE) << "Test sample validation: The test sample must contain at least three points. Here size * k / 100 = " << (inputSize * getTestSampleValidationPercentageOfPoints() / 100);
  if (kFoldValidation() && inputSize / getKFoldValidationNumberOfFolds() < 3)
    throw InvalidArgumentException(HERE) << "K-Fold validation: each fold must contain at least three points. Here size / k = " << (inputSize / getKFoldValidationNumberOfFolds());
  const Point stddev(effectiveOutputSample.computeStandardDeviation());
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    if (!(stddev[i] > 0.0))
      throw InvalidArgumentException(HERE) << "No variance for output variable " << outputVariables[i];

  // Kriging

  // temp
  Collection<KrigingAnalysisResult> allResults;

  // for each output:
  // because with all outputs KrigingAlgorithm is not reliable
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    if (stopRequested_)
      break;

    informationMessage_ = "Creation of a meta model for the variable " + outputVariables[i] + " in progress.\n";
    notifyMessageUpdated();

    // build algo
    GaussianProcessRegression gpr(buildGPRAlgorithm(effectiveInputSample, effectiveOutputSample.getMarginal(i), optimizeParameters_));

    // run algo
    gpr.run();

    // get results
    KrigingAnalysisResult result_i;
    result_i.outputSample_ = effectiveOutputSample.getMarginal(i);
    GaussianProcessRegressionResult result(gpr.getResult());
    result_i.gprResultCollection_.add(result);

    optimalCovarianceModel_ = result.getCovarianceModel();

    allResults.add(result_i);
  }

  const UnsignedInteger effectiveDim = allResults.getSize();

  // set result_
  Function::FunctionCollection metaModelCollection;
  Indices computedOutputIndices(effectiveDim);
  computedOutputIndices.fill();
  result_.outputSample_ = effectiveOutputSample.getMarginal(computedOutputIndices);
  for (UnsignedInteger i = 0; i < effectiveDim; ++i)
  {
    result_.gprResultCollection_.add(allResults[i].gprResultCollection_[0]);
    metaModelCollection.add(allResults[i].gprResultCollection_[0].getMetaModel());
  }

  // build metamodel
  AggregatedFunction metamodelFunction(metaModelCollection);
  Description variablesNames(inputVariables);
  variablesNames.add(result_.outputSample_.getDescription());
  metamodelFunction.setDescription(variablesNames);

  buildMetaModel(result_, metamodelFunction);
  result_.metaModelOutputSample_ = metamodelFunction(effectiveInputSample);
  notify("metamodelAvailable");

  // validations
  validateMetaModelResult(allResults, effectiveInputSample);
}


Function KrigingAnalysis::runAlgoMarginal(const Sample& inputSample, const Sample& outputSample)
{
  GaussianProcessRegression gpr(buildGPRAlgorithm(inputSample, outputSample, false, true));
  gpr.run();
  return gpr.getResult().getMetaModel();
}


GaussianProcessRegression KrigingAnalysis::buildGPRAlgorithm(const Sample& inputSample,
    const Sample& outputSample,
    const bool optimizeParameters,
    const bool useOptimalCovModel)
{
  if (outputSample.getDimension() != 1)
    throw InternalException(HERE) << "KrigingAnalysis::buildGPRAlgorithm: the output sample must have a dimension of 1";
  if (useOptimalCovModel && (optimalCovarianceModel_.getOutputDimension() > 1))
    throw InternalException(HERE) << "KrigingAnalysis::buildGPRAlgorithm: the optimal covariance model must have a dimension of 1";

  GaussianProcessFitter fitter(inputSample,
                        outputSample,
                        useOptimalCovModel ? optimalCovarianceModel_ : covarianceModel_,
                        getBasis());
  fitter.setOptimizeParameters(optimizeParameters);
  fitter.run();
  GaussianProcessRegression algo(fitter.getResult());
  return algo;
}


void KrigingAnalysis::validateMetaModelResult(Collection<KrigingAnalysisResult> results, const Sample& inputSample)
{
  // validation: Analytical
  if (analyticalValidation_)
  {
    try
    {
      for (UnsignedInteger i = 0; i < results.getSize(); ++i)
      {
        optimalCovarianceModel_ = results[i].gprResultCollection_[0].getCovarianceModel();
        computeAnalyticalValidation(results[i], inputSample);
      }
    }
    catch (const std::exception & ex)
    {
      analyticalValidation_ = false;
      warningMessage_ += OSS() << "Analytical validation failed: " << ex.what() << "\n";
    }
  }
  // validation: Test sample
  if (testSampleValidation_)
  {
    try
    {
      for (UnsignedInteger i = 0; i < results.getSize(); ++i)
      {
        optimalCovarianceModel_ = results[i].gprResultCollection_[0].getCovarianceModel();
        computeTestSampleValidation(results[i], inputSample);
      }
    }
    catch (const std::exception & ex)
    {
      testSampleValidation_ = false;
      warningMessage_ += OSS() << "Test sample validation failed: " << ex.what() << "\n";
    }
  }
  // validation: K-Fold
  if (kFoldValidation_)
  {
    try
    {
      for (UnsignedInteger i = 0; i < results.getSize(); ++i)
      {
        optimalCovarianceModel_ = results[i].gprResultCollection_[0].getCovarianceModel();
        computeKFoldValidation(results[i], inputSample);
      }
    }
    catch (const std::exception & ex)
    {
      kFoldValidation_ = false;
      warningMessage_ += OSS() << "K-Fold validation failed: " << ex.what() << "\n";
    }
  }
  // validation: Leave-one-out
  if (leaveOneOutValidation_)
  {
    try
    {
      for (UnsignedInteger i = 0; i < results.getSize(); ++i)
      {
        optimalCovarianceModel_ = results[i].gprResultCollection_[0].getCovarianceModel();
        computeLOOValidation(results[i], inputSample);
      }
    }
    catch (const std::exception & ex)
    {
      leaveOneOutValidation_ = false;
      warningMessage_ += OSS() << "Leave-one-out validation failed: " << ex.what() << "\n";
    }
  }
  if (analyticalValidation_ || testSampleValidation_ || kFoldValidation_ || leaveOneOutValidation_)
  {
    for (UnsignedInteger i = 0; i < results.getSize(); ++i)
    {
      if (analyticalValidation_)
        result_.analyticalValidation_.add(results[i].analyticalValidation_);
      if (testSampleValidation_)
        result_.testSampleValidation_.add(results[i].testSampleValidation_);
      if (kFoldValidation_)
        result_.kFoldValidation_.add(results[i].kFoldValidation_);
      if (leaveOneOutValidation_)
        result_.looValidation_.add(results[i].looValidation_);
    }
  }
}


void KrigingAnalysis::computeAnalyticalValidation(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  if (stopRequested_)
    return;

  informationMessage_ = "The analytical validation is running.";
  notifyMessageUpdated();

  // retrieve kriging result
  const GaussianProcessRegressionResult gprResult(dynamic_cast<KrigingAnalysisResult*>(&result)->getGPRResultCollection()[0]);
  const UnsignedInteger size = inputSample.getSize();

  // correlation matrix
  CovarianceMatrix R(gprResult.getCovarianceModel().discretize(inputSample));
  const TriangularMatrix C(R.computeCholesky());
  const SquareMatrix K(C * C.transpose());

  // F
  const Basis basis = gprResult.getBasis();
  const UnsignedInteger basisDim = basis.getSize();
  Sample F(size, 0);
  for (UnsignedInteger i = 0; i < basisDim; ++i)
    F.stack(basis[i](inputSample));

  // S
  SquareMatrix S(size + basisDim);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < size; ++j)
      S(i, j) = K(i, j);

    for (UnsignedInteger j = 0; j < basisDim; ++j)
    {
      S(size + j, i) = F(i, j);
      S(i, size + j) = F(i, j);
    }
  }

  // S^{-1}
  const SquareMatrix S_inv(S.solveLinearSystem(IdentityMatrix(size + basisDim)).getImplementation());

  // metamodel values
  result.analyticalValidation_.metaModelSample_ = Sample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < size; ++j)
      if (i != j)
        result.analyticalValidation_.metaModelSample_(i, 0) -= S_inv(i, j) / S_inv(i, i) * result.outputSample_(j, 0);

  // Compute Q2
  computeError(result.analyticalValidation_.metaModelSample_, result.outputSample_, result.analyticalValidation_.mse_, result.analyticalValidation_.q2_);
}

PhysicalModel KrigingAnalysis::getMetaModel() const
{
  return result_.getMetaModel();
}

KrigingAnalysisResult KrigingAnalysis::getResult() const
{
  return result_;
}


Parameters KrigingAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Gaussian Process");
  param.add("Outputs of interest", getInterestVariables().__str__());

  param.add("Covariance model", getCovarianceModel().getImplementation()->getClassName());
  if (getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
  {
    const Scalar nu = dynamic_cast<MaternModel*>(getCovarianceModel().getImplementation().get())->getNu();
    param.add("nu", nu);
  }
  else if (getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
  {
    const Scalar p = dynamic_cast<GeneralizedExponential*>(getCovarianceModel().getImplementation().get())->getP();
    param.add("p", p);
  }
  param.add("Parameters optimization", getOptimizeParameters() ? "yes" : "no");
  param.add("Scale", getCovarianceModel().getScale());
  param.add("Amplitude", getCovarianceModel().getAmplitude());
  String basisType("Constant");
  const UnsignedInteger dim = getBasis().getInputDimension();
  if (getBasis().getSize() == (dim + 1))
    basisType = "Linear";
  else if (getBasis().getSize() == ((dim + 1) * (dim + 2) / 2))
    basisType = "Quadratic";
  param.add("Trend basis", basisType);

  param.add("Analytical validation", analyticalValidation() ? "yes" : "no");
  param.add("Test sample validation", testSampleValidation() ? "yes" : "no");
  if (testSampleValidation())
  {
    param.add("Percentage", getTestSampleValidationPercentageOfPoints());
    param.add("Seed", getTestSampleValidationSeed());
  }
  param.add("KFold validation", kFoldValidation() ? "yes" : "no");
  if (kFoldValidation())
  {
    param.add("Number of folds", getKFoldValidationNumberOfFolds());
    param.add("Seed", getKFoldValidationSeed());
  }
//   param.add("Leave-one-out validation", leaveOneOutValidation() ? "yes" : "no");

  return param;
}


String KrigingAnalysis::getPythonScript() const
{
  OSS oss;

  oss << getName() << " = persalys.KrigingAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

  // interest outputs
  if (getInterestVariables().getSize() < getDesignOfExperiment().getOutputSample().getDimension())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  // basis
  const UnsignedInteger effectiveInputDim = getCovarianceModel().getScale().getSize();
  const UnsignedInteger basisDim = getBasis().getInputDimension();

  oss << getName() << ".setBasis(";
  if (getBasis().getSize() == (basisDim + 1))
    oss << "ot.LinearBasisFactory(" << effectiveInputDim << ").build()";
  else if (getBasis().getSize() == ((basisDim + 1) * (basisDim + 2) / 2))
    oss << "ot.QuadraticBasisFactory(" << effectiveInputDim << ").build()";
  else
    oss << "ot.ConstantBasisFactory(" << effectiveInputDim << ").build()";
  oss << ")\n";

  // covariance model
  oss << getName() << ".setCovarianceModel(ot." << getCovarianceModel().getImplementation()->getClassName()
      << "(" << Parameters::GetOTPointStr(getCovarianceModel().getScale()) << ", "
      << Parameters::GetOTPointStr(getCovarianceModel().getAmplitude());

  if (getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
    oss << ", " << dynamic_cast<MaternModel*>(getCovarianceModel().getImplementation().get())->getNu();
  else if (getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
    oss << ", " << dynamic_cast<GeneralizedExponential*>(getCovarianceModel().getImplementation().get())->getP();
  oss << "))\n";

  oss << getName() << ".setOptimizeParameters(" << (getOptimizeParameters() ? "True" : "False") << ")\n";

  // validation
  oss << getName() << ".setAnalyticalValidation(" << (analyticalValidation() ? "True" : "False") << ")\n";
  oss << getName() << ".setTestSampleValidation(" << (testSampleValidation() ? "True" : "False") << ")\n";
  if (testSampleValidation())
  {
    oss << getName() << ".setTestSampleValidationPercentageOfPoints(" << getTestSampleValidationPercentageOfPoints() << ")\n";
    oss << getName() << ".setTestSampleValidationSeed(" << getTestSampleValidationSeed() << ")\n";
  }
  oss << getName() << ".setKFoldValidation(" << (kFoldValidation() ? "True" : "False") << ")\n";
  if (kFoldValidation())
  {
    oss << getName() << ".setKFoldValidationNumberOfFolds(" << getKFoldValidationNumberOfFolds() << ")\n";
    oss << getName() << ".setKFoldValidationSeed(" << getKFoldValidationSeed() << ")\n";
  }
//   oss << getName() << ".setLeaveOneOutValidation(" << (leaveOneOutValidation() ? "True" : "False") << ")\n";

  return oss;
}


bool KrigingAnalysis::hasValidResult() const
{
  return getResult().getMetaModelOutputSample().getSize();
}


/* String converter */
String KrigingAnalysis::__repr__() const
{
  OSS oss;
  oss << MetaModelAnalysis::__repr__()
      << " basis=" << getBasis()
      << " covarianceModel=" << getCovarianceModel()
      << " optimizeParameters=" << getOptimizeParameters();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void KrigingAnalysis::save(Advocate& adv) const
{
  MetaModelAnalysis::save(adv);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("optimizeParameters_", optimizeParameters_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void KrigingAnalysis::load(Advocate& adv)
{
  MetaModelAnalysis::load(adv);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("optimizeParameters_", optimizeParameters_);
  adv.loadAttribute("result_", result_);
}
}
