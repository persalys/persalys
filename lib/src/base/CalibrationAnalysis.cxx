//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "persalys/CalibrationAnalysis.hxx"
#include "persalys/DistributionDictionary.hxx"
#ifdef PERSALYS_HAVE_ADAO_CPP_LAYER
# include "persalys/AdaoCalibration.hxx"
#endif

#include <openturns/MemoizeFunction.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/LinearLeastSquaresCalibration.hxx>
#include <openturns/NonLinearLeastSquaresCalibration.hxx>
#include <openturns/GaussianLinearCalibration.hxx>
#include <openturns/GaussianNonLinearCalibration.hxx>
#include <openturns/Dirac.hxx>
#include <openturns/TNC.hxx>
#include <openturns/SobolSequence.hxx>
#include <openturns/Normal.hxx>
#include <openturns/LowDiscrepancyExperiment.hxx>
#include <openturns/MultiStart.hxx>
#include <openturns/KernelSmoothing.hxx>
#include <openturns/MarginalDistribution.hxx>
#include <openturns/NormalCopula.hxx>
#include <openturns/LeastSquaresProblem.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CalibrationAnalysis)

static Factory<CalibrationAnalysis> Factory_CalibrationAnalysis;

/* Default constructor */
CalibrationAnalysis::CalibrationAnalysis()
  : PhysicalModelAnalysis()
  , methodName_("LeastSquaresLinear")
{
}


/* Constructor with parameters */
CalibrationAnalysis::CalibrationAnalysis(const String &name, const DesignOfExperiment &observations)
  : PhysicalModelAnalysis(name, observations.getPhysicalModel())
  , methodName_("LeastSquaresLinear")
  , observations_(observations)
{
  if (!observations.hasPhysicalModel())
    throw InvalidArgumentException(HERE) << "The Design Of Experiment must be built from a physical model";
  initializeParameters();
}


/* Virtual constructor */
CalibrationAnalysis* CalibrationAnalysis::clone() const
{
  return new CalibrationAnalysis(*this);
}


Observer * CalibrationAnalysis::getParentObserver() const
{
  return observations_.getImplementation()->getObserver("ObservationsItem");
}


Description CalibrationAnalysis::MethodNames_;

Description CalibrationAnalysis::GetMethodNames()
{
  if (MethodNames_.isEmpty())
  {
    MethodNames_ = Description();
    MethodNames_.add("LeastSquaresLinear");
    MethodNames_.add("LeastSquaresNonlinear");
    MethodNames_.add("GaussianLinear");
    MethodNames_.add("GaussianNonlinear");
  }

  return MethodNames_;
}


void CalibrationAnalysis::initializeParameters()
{
  const Description inputNames(getPhysicalModel().getInputNames());
  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();
  const Description observedInputs(getObservations().getInputSample().getDescription());
  const UnsignedInteger nbOutputs = getObservations().getOutputSample().getDimension();
  if (observedInputs.getSize() == nbInputs)
    throw InvalidValueException(HERE) << "At least an input variable must be calibrated. Here, all the input variables are observed.";

  // set prior distribution
  Point candidate;
  ComposedDistribution::DistributionCollection marginals;
  Description calibratedInputs;
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    if (!observedInputs.contains(inputNames[i]))
    {
      calibratedInputs.add(inputNames[i]);
      const Scalar value_i = getPhysicalModel().getInputs()[i].getValue();
      candidate.add(value_i);
      marginals.add(Dirac(value_i));
    }
  }
  priorDistribution_ = ComposedDistribution(marginals);
  priorDistribution_.setDescription(calibratedInputs);

  // set error covariance
  errorCovariance_ = CovarianceMatrix(nbOutputs);
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    errorCovariance_(i, i) = 0.01;

  // automatic selection of the optimization algorithm
  optimizationAlgorithm_ = OptimizationAlgorithm::Build(LeastSquaresProblem());
}


void CalibrationAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = CalibrationAnalysisResult();
}


void CalibrationAnalysis::launch()
{
  // check
  check(getCalibratedInputs(), priorDistribution_, getFixedInputs().getDescription(), fixedValues_);
  if (errorCovariance_.getDimension() != getObservations().getOutputSample().getDimension())
    throw InvalidArgumentException(HERE) << "The error covariance matrix dimension must equel to the number of observed outputs";

  const Description modelInputNames(getPhysicalModel().getInputNames());
  const Description observedInputNames(getObservations().getInputSample().getDescription());
  for (UnsignedInteger i = 0; i < observedInputNames.getSize(); ++i)
  {
    if (!modelInputNames.contains(observedInputNames[i]))
      throw InvalidArgumentException(HERE) << "The model does not contain the observed input named: " << observedInputNames[i];
  }

  // theta prior and output of interest (= observed output)
  const Point thetaPrior(priorDistribution_.getMean());
  const Description outputsOfInterest(getObservations().getOutputSample().getDescription());

  // get fixed input indices
  Indices fixedInputIndices;
  for (UnsignedInteger i = 0; i < getFixedInputs().getDescription().getSize(); ++i)
  {
    const Description::const_iterator it = std::find(modelInputNames.begin(), modelInputNames.end(), getFixedInputs().getDescription()[i]);
    fixedInputIndices.add(it - modelInputNames.begin());
  }

  // get function
  Function modelFunction(getPhysicalModel().getFunction(outputsOfInterest));
  if (fixedInputIndices.getSize())
    modelFunction = ParametricFunction(modelFunction, fixedInputIndices, fixedValues_);

  // get calibrated input indices
  Indices calibratedInputIndices;
  const Description modelInputNames2(modelFunction.getInputDescription());
  for (UnsignedInteger i = 0; i < getCalibratedInputs().getSize(); ++i)
  {
    const Description::const_iterator it = std::find(modelInputNames2.begin(), modelInputNames2.end(), getCalibratedInputs()[i]);
    calibratedInputIndices.add(it - modelInputNames2.begin());
  }

  // set parametric function for calibration
  ParametricFunction paramFunction(modelFunction, calibratedInputIndices, thetaPrior);

  // run calibration
  runCalibrationAlgorithm(paramFunction);

  // Post processing

  // get posterior distribution
  const Distribution distributionPosterior(result_.calibrationResult_.getParameterPosterior());

  // compute confidence intervals
  // -- use workaround to avoid error in computeBilateralConfidenceInterval
  const UnsignedInteger oldValue1 = ResourceMap::GetAsUnsignedInteger("Normal-SmallDimension");
  ResourceMap::SetAsUnsignedInteger("Normal-SmallDimension", 1);
  // -- use workaround to avoid too long computation
  const UnsignedInteger oldValue2 = ResourceMap::GetAsUnsignedInteger("Normal-MinimumNumberOfPoints");
  ResourceMap::SetAsUnsignedInteger("Normal-MinimumNumberOfPoints", 10000);
  try
  {
    result_.confidenceInterval_ = distributionPosterior.computeBilateralConfidenceInterval(confidenceIntervalLength_);
  }
  catch (const std::exception &)
  {
    LOGWARN("Cannot compute the confidence interval");
  }
  ResourceMap::SetAsUnsignedInteger("Normal-SmallDimension", oldValue1);
  ResourceMap::SetAsUnsignedInteger("Normal-MinimumNumberOfPoints", oldValue2);

  KernelSmoothing kernel;
  result_.priorResidualsPDF_ = Collection<Sample>(outputsOfInterest.getSize());
  result_.posteriorResidualsPDF_ = Collection<Sample>(outputsOfInterest.getSize());

  // Prior

  // - residuals kernel smoothing
  const Sample priorResiduals(getObservations().getOutputSample() - result_.calibrationResult_.getOutputAtPriorMean());
  for (UnsignedInteger i = 0; i < outputsOfInterest.getSize(); ++i)
  {
    try
    {
      result_.priorResidualsPDF_[i] = kernel.build(priorResiduals.getMarginal(i)).drawPDF().getDrawables()[0].getData();
    }
    catch (const std::exception &)
    {
      LOGWARN("Cannot build prior residual PDF");
    }
  }

  // Posterior

  // - residuals kernel smoothing
  const Sample posteriorResiduals(getObservations().getOutputSample() - result_.calibrationResult_.getOutputAtPosteriorMean());
  for (UnsignedInteger i = 0; i < outputsOfInterest.getSize(); ++i)
  {
    try
    {
      result_.posteriorResidualsPDF_[i] = kernel.build(posteriorResiduals.getMarginal(i)).drawPDF().getDrawables()[0].getData();
    }
    catch (const std::exception &)
    {
      LOGWARN("Cannot build posterior residual PDF");
    }
  }
}


void CalibrationAnalysis::runCalibrationAlgorithm(const Function &paramFunction)
{
  const Point thetaPrior(priorDistribution_.getMean());
  const CovarianceMatrix priorCovariance(priorDistribution_.getCovariance());

#ifdef PERSALYS_HAVE_ADAO_CPP_LAYER
  char *calibrationEngineSystem(std::getenv("PERSALYS_CALIBRATION_ENGINE"));
  String calibrationEngine;
  if(calibrationEngineSystem)
    calibrationEngine = calibrationEngineSystem;
  if (calibrationEngine == "adao")
  {
    AdaoCalibration algo(getMethodName(),
                         paramFunction,
                         getObservations().getInputSample(),
                         getObservations().getOutputSample(),
                         thetaPrior,
                         priorCovariance,
                         errorCovariance_);
    algo.run();
    result_.calibrationResult_ = algo.getResult();
  }
  else
#endif
  {
    if (getMethodName() == "LeastSquaresLinear")
    {
      LinearLeastSquaresCalibration algo(paramFunction,
                                         getObservations().getInputSample(),
                                         getObservations().getOutputSample(),
                                         thetaPrior,
                                         ResourceMap::GetAsString("LinearLeastSquaresCalibration-Method"));
      algo.run();
      result_.calibrationResult_ = algo.getResult();
    }
    else if (getMethodName() == "GaussianLinear")
    {
      GaussianLinearCalibration algo(paramFunction,
                                     getObservations().getInputSample(),
                                     getObservations().getOutputSample(),
                                     thetaPrior,
                                     priorCovariance,
                                     errorCovariance_,
                                     ResourceMap::GetAsString("GaussianLinearCalibration-Method"));
      algo.run();
      result_.calibrationResult_ = algo.getResult();
    }
    else if (getMethodName() == "LeastSquaresNonlinear")
    {
      NonLinearLeastSquaresCalibration algo(paramFunction,
                                            getObservations().getInputSample(),
                                            getObservations().getOutputSample(),
                                            thetaPrior);
      runNonLinearAlgorithm(algo);
    }
    else if (getMethodName() == "GaussianNonlinear")
    {
      GaussianNonLinearCalibration algo(paramFunction,
                                        getObservations().getInputSample(),
                                        getObservations().getOutputSample(),
                                        thetaPrior,
                                        priorCovariance,
                                        errorCovariance_);
      runNonLinearAlgorithm(algo);
    }
  }
}


template <class T>
void CalibrationAnalysis::runNonLinearAlgorithm(T& algo)
{
  OptimizationAlgorithm optimAlgo(getOptimizationAlgorithm());
  optimAlgo.setStopCallback(&AnalysisImplementation::Stop, this);
  algo.setOptimizationAlgorithm(optimAlgo);
  algo.setBootstrapSize(bootStrapSize_);
  algo.run();
  result_.calibrationResult_ = algo.getResult();
}


void CalibrationAnalysis::check(const Description &calibratedInputs, const Distribution &priorDistribution,
                                const Description &fixedInputs, const Point &fixedValues)
{
  // dimensions
  if (!calibratedInputs.getSize())
    throw InvalidArgumentException(HERE) << "At least an input variable must be calibrated.";
  if (calibratedInputs.getSize() != priorDistribution.getDimension())
    throw InvalidArgumentException(HERE) << "The dimension of the prior distribution must equal to the number of inputs to calibrate";
  if (fixedInputs.getSize() != fixedValues.getSize())
    throw InvalidArgumentException(HERE) << "The dimension of the list of fixed values must equal to the number of fixed inputs";

  // check input names
  const Description inputNames(getPhysicalModel().getInputNames());
  const Description observedInputNames(getObservations().getInputSample().getDescription());
  for (UnsignedInteger i = 0; i < calibratedInputs.getSize(); ++i)
  {
    if (!inputNames.contains(calibratedInputs[i]))
      throw InvalidArgumentException(HERE) << "The model does not contain the calibrated input named: " << calibratedInputs[i];
    if (fixedInputs.contains(calibratedInputs[i]))
      throw InvalidArgumentException(HERE) << "An input " << calibratedInputs[i] << " can not be fixed and calibrated at the same time";
    if (observedInputNames.contains(calibratedInputs[i]))
      throw InvalidArgumentException(HERE) << "An observed input " << calibratedInputs[i] << " can not be calibrated";
  }
  for (UnsignedInteger i = 0; i < fixedInputs.getSize(); ++i)
  {
    if (!inputNames.contains(fixedInputs[i]))
      throw InvalidArgumentException(HERE) << "The model does not contain the fixed input named: " << fixedInputs[i];
    if (observedInputNames.contains(fixedInputs[i]))
      throw InvalidArgumentException(HERE) << "An observed input " << fixedInputs[i] << " can not be fixed";
  }

  // check the prior distribution
  if (getMethodName().compare(0, 8, "Gaussian") == 0)
  {
    for (UnsignedInteger i = 0; i < priorDistribution.getDimension(); ++i)
      if (priorDistribution.getMarginal(i).getImplementation()->getClassName() != "Normal")
        throw InvalidArgumentException(HERE) << "The prior distribution of the gaussian calibration algorithms must be gaussian";
    // if distribution has dependence : check the copula is Gaussian
    if (!priorDistribution.hasIndependentCopula())
    {
      Distribution copula(priorDistribution.getCopula());
      // can have copula = NormalCopula / ComposedCopula / MarginalDistribution
      MarginalDistribution * margDist = dynamic_cast<MarginalDistribution *>(copula.getImplementation().get());
      if (margDist)
        copula = margDist->getDistribution();
      ComposedCopula * composedCopula = dynamic_cast<ComposedCopula *>(copula.getImplementation().get());

      bool normalCopulaFound = false;
      // check if composedCopula contains only NormalCopula (with potentially IndependentCopula)
      if (composedCopula)
      {
        for (UnsignedInteger i = 0; i < composedCopula->getCopulaCollection().getSize(); ++i)
        {
          const String copulaName = composedCopula->getCopulaCollection()[i].getImplementation()->getClassName();
          if (copulaName == "NormalCopula")
            normalCopulaFound = true;
          if (copulaName != "IndependentCopula" && copulaName != "NormalCopula")
          {
            normalCopulaFound = false;
            break;
          }
        }
      }
      else
        normalCopulaFound = dynamic_cast<NormalCopula *>(copula.getImplementation().get()) != 0;
      if (!normalCopulaFound)
        throw InvalidArgumentException(HERE) << "The dependence structure of the prior distribution must be gaussian";
    }
  }
  else
  {
    if (!priorDistribution.isDiscrete())
      throw InvalidArgumentException(HERE) << "The prior distribution of the least square calibration algorithms must be a Dirac";
  }
}


void CalibrationAnalysis::setCalibratedInputs(const Description &calibratedInputs, const Distribution &priorDistribution,
    const Description &fixedInputs, const Point &fixedValues)
{
  // check arguments
  check(calibratedInputs, priorDistribution, fixedInputs, fixedValues);

  priorDistribution_ = priorDistribution;
  priorDistribution_.setDescription(calibratedInputs);
  fixedValues_ = fixedValues;
  fixedValues_.setDescription(fixedInputs);
}


void CalibrationAnalysis::setErrorCovariance(const CovarianceMatrix &matrix)
{
  if (matrix.getDimension() != getObservations().getOutputSample().getDimension())
    throw InvalidArgumentException(HERE) << "The error covariance matrix dimension must equal to the number of observed outputs";
  errorCovariance_ = matrix;
}


void CalibrationAnalysis::setMethodName(const String &newMethod)
{
  if (!GetMethodNames().contains(newMethod))
    throw InvalidArgumentException(HERE) << "Error: the given method name=" << newMethod << " is unknown. " << GetMethodNames();

  // update prior distribution if modification of method
  ComposedDistribution::DistributionCollection marginals;
  const Description calibratedInputs(priorDistribution_.getDescription());
  const Point oldValues(priorDistribution_.getMean());
  // old method = Gaussian / new method = Least Squares => prior dist = Dirac
  if (getMethodName().compare(0, 8, "Gaussian") == 0 && newMethod.compare(0, 8, "Gaussian") != 0)
  {
    for (UnsignedInteger i = 0; i < oldValues.getDimension(); ++i)
    {
      marginals.add(Dirac(oldValues[i]));
    }
    priorDistribution_ = ComposedDistribution(marginals);
    priorDistribution_.setDescription(calibratedInputs);
  }
  // old method = Least Squares / new method = Gaussian => prior dist = Gaussian
  else if (getMethodName().compare(0, 8, "Gaussian") != 0 && newMethod.compare(0, 8, "Gaussian") == 0)
  {
    for (UnsignedInteger i = 0; i < oldValues.getDimension(); ++i)
    {
      Distribution normal(DistributionDictionary::BuildDistribution("Normal", oldValues[i]));
      marginals.add(normal);
    }
    priorDistribution_ = ComposedDistribution(marginals);
    priorDistribution_.setDescription(calibratedInputs);
  }

  methodName_ = newMethod;
}


void CalibrationAnalysis::setOptimizationAlgorithm(const OptimizationAlgorithm &solver)
{
  optimizationAlgorithm_ = solver;
}


void CalibrationAnalysis::setConfidenceIntervalLength(const Scalar length)
{
  confidenceIntervalLength_ = length;
}


void CalibrationAnalysis::setBootStrapSize(const UnsignedInteger size)
{
  bootStrapSize_ = size;
}


Parameters CalibrationAnalysis::getParameters() const
{
  Parameters param;

  param.add("Method", getMethodName());
  param.add("Observed variables", getObservations().getSample().getDescription().__str__());
  param.add("Variables to calibrate", getCalibratedInputs().__str__());
  if (getMethodName().compare(0, 8, "Gaussian") == 0)
  {
    String priorDist;
    for (UnsignedInteger i = 0; i < priorDistribution_.getDimension(); ++i)
    {
      priorDist += priorDistribution_.getDescription()[i] + " : " + priorDistribution_.getMarginal(i).__str__();
      if (i < priorDistribution_.getDimension() - 1)
        priorDist += "\n";
    }
    param.add("θ prior distribution", priorDist);
    if (!priorDistribution_.hasIndependentCopula())
      param.add("θ prior\nSpearman correlation", priorDistribution_.getSpearmanCorrelation().__str__());

    param.add("Error\ncovariance", errorCovariance_.__str__());
  }
  else
  {
    param.add("θ prior", priorDistribution_.getMean());
  }
  if (fixedValues_.getSize())
    param.add("Fixed inputs", fixedValues_.__str__());

  param.add("Confidence interval length", getConfidenceIntervalLength());
  if (getMethodName().compare(getMethodName().size() - 9, 9, "Nonlinear") == 0)
  {
    param.add("Bootstrap resampling size", getBootStrapSize());
    param.add("Maximum number of function evaluations", getOptimizationAlgorithm().getMaximumEvaluationNumber());
    param.add("Maximum absolute error", getOptimizationAlgorithm().getMaximumAbsoluteError());
    param.add("Maximum relative error", getOptimizationAlgorithm().getMaximumRelativeError());
    param.add("Maximum residual error", getOptimizationAlgorithm().getMaximumResidualError());
    param.add("Maximum constraint error", getOptimizationAlgorithm().getMaximumConstraintError());
  }

  return param;
}


String CalibrationAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = persalys." << getClassName() << "('" << getName() << "', " << getObservations().getName() << ")\n";
  oss << getName() << ".setMethodName('" << getMethodName() << "')\n";
  if (priorDistribution_.isDiscrete())
    oss << "priorDistribution = ot.Dirac(" << Parameters::GetOTPointStr(priorDistribution_.getMean()) << ")\n";
  else
  {
    oss <<  Parameters::GetOTCorrelationMatrixStr(priorDistribution_.getCorrelation());
    oss << "priorDistribution = ot.Normal(" << Parameters::GetOTPointStr(priorDistribution_.getMean()) << ", "
        << Parameters::GetOTPointStr(priorDistribution_.getStandardDeviation()) << ", R)\n";
  }
  oss << getName() << ".setCalibratedInputs(" << Parameters::GetOTDescriptionStr(getCalibratedInputs())
      << ", priorDistribution"
      << ", " << Parameters::GetOTDescriptionStr(getFixedInputs().getDescription())
      << ", " << Parameters::GetOTPointStr(getFixedInputs())
      << ")\n";
  oss << getName() << ".setConfidenceIntervalLength(" << getConfidenceIntervalLength() << ")\n";

  if (getMethodName().compare(0, 8, "Gaussian") == 0)
  {
    oss << "covMatrix = ot.CovarianceMatrix(" << errorCovariance_.getNbRows() << ")\n";

    for (UnsignedInteger row = 0; row < errorCovariance_.getNbRows(); ++ row)
    {
      for (UnsignedInteger col = 0; col < errorCovariance_.getNbRows(); ++ col)
      {
        if (errorCovariance_(row, col) != 0.0)
        {
          oss << "covMatrix[" << row << ", " << col << "] = " << errorCovariance_(row, col) << "\n";
        }
      }
    }
    oss << getName() << ".setErrorCovariance(covMatrix)\n";
  }

  if (getMethodName().compare(getMethodName().size() - 9, 9, "Nonlinear") == 0)
  {
    oss << getName() << ".setBootStrapSize(" << getBootStrapSize() << ")\n";
    // optimization algo
    oss << "optimizationAlgo = " << getName() << ".getOptimizationAlgorithm()\n";
    oss << "optimizationAlgo.setMaximumEvaluationNumber(" << getOptimizationAlgorithm().getMaximumEvaluationNumber() << ")\n";
    oss << "optimizationAlgo.setMaximumAbsoluteError(" << getOptimizationAlgorithm().getMaximumAbsoluteError() << ")\n";
    oss << "optimizationAlgo.setMaximumRelativeError(" << getOptimizationAlgorithm().getMaximumRelativeError() << ")\n";
    oss << "optimizationAlgo.setMaximumResidualError(" << getOptimizationAlgorithm().getMaximumResidualError() << ")\n";
    oss << "optimizationAlgo.setMaximumConstraintError(" << getOptimizationAlgorithm().getMaximumConstraintError() << ")\n";
    oss << getName() << ".setOptimizationAlgorithm(optimizationAlgo)\n";
  }

  return oss;
}


bool CalibrationAnalysis::hasValidResult() const
{
  return result_.calibrationResult_.getParameterMAP().getSize() != 0;
}


bool CalibrationAnalysis::canBeLaunched(String &errorMessage) const
{
  const bool canBeLaunched = PhysicalModelAnalysis::canBeLaunched(errorMessage);
  if (!canBeLaunched)
    return false;

  if (!getObservations().getInputSample().getDimension() || !getObservations().getOutputSample().getDimension())
    errorMessage = "Observations must be defined for at least an input and an output.";

  if (getObservations().getInputSample().getDimension() >= getObservations().getPhysicalModel().getInputDimension())
    errorMessage = "All the input variables can not be observed. At least an input variable must be calibrated.";

  return errorMessage.empty();
}


/* String converter */
String CalibrationAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << " methodName=" << getMethodName()
      << " calibratedInputs=" << getCalibratedInputs()
      << " priorDistribution=" << getPriorDistribution()
      << " fixedInputs=" << getFixedInputs()
      << " confidenceIntervalLength=" << getConfidenceIntervalLength();
  if (getMethodName().compare(0, 8, "Gaussian") == 0)
    oss << " errorCovariance=" << getErrorCovariance();
  if (getMethodName().compare(getMethodName().size() - 9, 9, "Nonlinear") == 0)
  {
    oss << " bootStrapSize=" << getBootStrapSize()
        << " maximumEvaluationNumber=" << getOptimizationAlgorithm().getMaximumEvaluationNumber()
        << " maximumAbsoluteError=" << getOptimizationAlgorithm().getMaximumAbsoluteError()
        << " maximumRelativeError=" << getOptimizationAlgorithm().getMaximumRelativeError()
        << " maximumResidualError=" << getOptimizationAlgorithm().getMaximumResidualError()
        << " maximumConstraintError=" << getOptimizationAlgorithm().getMaximumConstraintError();
  }
  return oss;
}


/* Method save() stores the object through the StorageManager */
void CalibrationAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("observations_", observations_);
  adv.saveAttribute("methodName_", methodName_);
  adv.saveAttribute("confidenceIntervalLength_", confidenceIntervalLength_);
  adv.saveAttribute("bootStrapSize_", bootStrapSize_);
  adv.saveAttribute("optimizationAlgorithm_", optimizationAlgorithm_);
  adv.saveAttribute("priorDistribution_", priorDistribution_);
  adv.saveAttribute("fixedValues_", fixedValues_);
  adv.saveAttribute("errorCovariance_", errorCovariance_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void CalibrationAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("observations_", observations_);
  adv.loadAttribute("methodName_", methodName_);
  adv.loadAttribute("confidenceIntervalLength_", confidenceIntervalLength_);
  adv.loadAttribute("bootStrapSize_", bootStrapSize_);
  adv.loadAttribute("optimizationAlgorithm_", optimizationAlgorithm_);
  adv.loadAttribute("priorDistribution_", priorDistribution_);
  adv.loadAttribute("fixedValues_", fixedValues_);
  adv.loadAttribute("errorCovariance_", errorCovariance_);
  adv.loadAttribute("result_", result_);
}
}
