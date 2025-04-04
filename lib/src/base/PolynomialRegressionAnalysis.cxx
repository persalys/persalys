//                                               -*- C++ -*-
/**
 *  @brief Linear metamodel
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
#include "persalys/PolynomialRegressionAnalysis.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/LinearModelStepwiseAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/AggregatedFunction.hxx>
#include <openturns/SymbolicFunction.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(PolynomialRegressionAnalysis)

static Factory<PolynomialRegressionAnalysis> Factory_PolynomialRegressionAnalysis;

/* Default constructor */
PolynomialRegressionAnalysis::PolynomialRegressionAnalysis()
  : MetaModelAnalysis()
{

}


/* Constructor with parameters */
PolynomialRegressionAnalysis::PolynomialRegressionAnalysis(const String& name, const DesignOfExperiment& designOfExperiment)
  : MetaModelAnalysis(name, designOfExperiment)
{
}


/* Constructor with parameters */
PolynomialRegressionAnalysis::PolynomialRegressionAnalysis(const String& name, const Analysis& analysis)
  : MetaModelAnalysis(name, analysis)
{
}


/* Virtual constructor */
PolynomialRegressionAnalysis* PolynomialRegressionAnalysis::clone() const
{
  return new PolynomialRegressionAnalysis(*this);
}

UnsignedInteger PolynomialRegressionAnalysis::getDegree() const
{
  return degree_;
}


void PolynomialRegressionAnalysis::setDegree(const UnsignedInteger degree)
{
  degree_ = degree;
}


Bool PolynomialRegressionAnalysis::getInteraction() const
{
  return interaction_;
}


void PolynomialRegressionAnalysis::setInteraction(const Bool interaction)
{
  interaction_ = interaction;
}


void PolynomialRegressionAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = PolynomialRegressionAnalysisResult();
}


void PolynomialRegressionAnalysis::launch()
{
  // get effective samples
  const Sample effectiveInputSample(getEffectiveInputSample());
  const Sample effectiveOutputSample(getEffectiveOutputSample());
  const UnsignedInteger inputSize = effectiveInputSample.getSize();
  const UnsignedInteger inputDimension = effectiveInputSample.getDimension();
  const UnsignedInteger outputDimension = effectiveOutputSample.getDimension();
  const Description inputVariables(effectiveInputSample.getDescription());

  // check
  if (inputSize * designOfExperiment_.getOutputSample().getSize() == 0)
    throw InvalidArgumentException(HERE) << "The design of experiments must contains not empty input AND output samples";
  if (inputSize != designOfExperiment_.getOutputSample().getSize())
    throw InvalidArgumentException(HERE) << "The input sample and the output sample must have the same size";
  const Point stddev(effectiveOutputSample.computeStandardDeviation());
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    if (!(stddev[i] > 0.0))
      throw InvalidArgumentException(HERE) << "No variance for output variable " << i;

  // for each output:
  Collection<LinearModelResult> allResults;
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
  {
    if (stopRequested_)
      break;

    informationMessage_ = "Creation of a meta model for the variable " + effectiveOutputSample.getDescription()[i] + " in progress.\n";
    notify("informationMessageUpdated");

    LinearModelStepwiseAlgorithm algo(buildAlgo(effectiveInputSample, effectiveOutputSample.getMarginal(i)));
    algo.run();
    allResults.add(algo.getResult());
  }

  const UnsignedInteger effectiveDim = allResults.getSize();
  // set result_
  Function::FunctionCollection metaModelCollection(effectiveDim);
  Indices computedOutputIndices(effectiveDim);
  computedOutputIndices.fill();
  result_.outputSample_ = effectiveOutputSample.getMarginal(computedOutputIndices);
  result_.linearModelResultCollection_ = allResults;
  Description formulas(effectiveDim);
  const Basis basis(getBasis());
  for (UnsignedInteger i = 0; i < effectiveDim; ++i)
  {
    // retrieve retained basis indices
    Indices basisIndices;
    for (UnsignedInteger k = 0; k < allResults[i].getBasis().getSize(); ++ k)
      for (UnsignedInteger j = 0; j < basis.getSize(); ++ j)
        if (allResults[i].getBasis()[k].__str__() == basis[j].__str__())
          basisIndices.add(j);

    OSS oss;
    const Point coefficients(allResults[i].getCoefficients());
    if (basisIndices.contains(0)) // constant term
      oss << coefficients[0];
    else
      oss << 0.0;
    // linear term
    for (UnsignedInteger j = 0; j < inputDimension; ++ j)
      for (UnsignedInteger k = 0; k < basisIndices.getSize(); ++ k)
        if (basisIndices[k] == j + 1)
          oss << " " << std::showpos << coefficients[k] << " * " << inputVariables[j];
    // quadratic term
    for (UnsignedInteger j = 0; j < inputDimension; ++ j)
      for (UnsignedInteger k = 0; k < basisIndices.getSize(); ++ k)
        if (basisIndices[k] == inputDimension + j + 1)
          oss << " " << std::showpos << coefficients[k] << " * " << inputVariables[j] << "^2";
    // cross term
    UnsignedInteger j = 0;
    for (UnsignedInteger j1 = 0; j1 < inputDimension; ++ j1)
      for (UnsignedInteger j2 = 0; j2 < j1; ++ j2)
      {
        for (UnsignedInteger k = 0; k < basisIndices.getSize(); ++ k)
          if (basisIndices[k] == 2 * inputDimension + j + 1)
            oss << " " << std::showpos << coefficients[k] << " * " << inputVariables[j1] << " * " << inputVariables[j2];
        ++ j;
      }
    formulas[i] = oss;
    metaModelCollection[i] = allResults[i].getMetaModel();
  }

  // build metamodel
  AggregatedFunction metamodelFunction(metaModelCollection);
  Description variablesNames(effectiveInputSample.getDescription());
  variablesNames.add(result_.outputSample_.getDescription());
  metamodelFunction.setDescription(variablesNames);

  buildMetaModel(result_, metamodelFunction);
  result_.metaModelOutputSample_ = metamodelFunction(effectiveInputSample);
  result_.formulas_ = formulas;
  notify("metamodelAvailable");

  // validations
  validateMetaModelResult(result_, effectiveInputSample);
}


Function PolynomialRegressionAnalysis::runAlgoMarginal(const Sample& inputSample, const Sample& outputSample)
{
  LinearModelStepwiseAlgorithm algo(buildAlgo(inputSample, outputSample));
  algo.run();
  return algo.getResult().getMetaModel();
}

Basis PolynomialRegressionAnalysis::getBasis() const
{
  // basis
  Collection<Function> functions;
  const UnsignedInteger inputDimension = getEffectiveInputSample().getDimension();
  Description inputVariables(Description::BuildDefault(inputDimension, "x"));
  functions.add(SymbolicFunction(Tools::GetNormalizedVariables(inputVariables), Description(1, "1")));
  // degree 1 terms
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    functions.add(SymbolicFunction(Tools::GetNormalizedVariables(inputVariables),
                                   Description(1, Tools::GetNormalizedVariable(inputVariables[j]))));
  // degree 2 terms
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    for (UnsignedInteger d = 2; d <= degree_; ++ d)
      functions.add(SymbolicFunction(Tools::GetNormalizedVariables(inputVariables),
                                     Description(1, OSS() << Tools::GetNormalizedVariable(inputVariables[j]) << "^" << d)));
  // degree 2 interactions
  if (interaction_ && (degree_ >= 2))
    for (UnsignedInteger i = 0; i < inputDimension; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        functions.add(SymbolicFunction(Tools::GetNormalizedVariables(inputVariables),
                                       Description(1, OSS() << Tools::GetNormalizedVariable(inputVariables[i]) << "*" << Tools::GetNormalizedVariable(inputVariables[j]))));
  const Basis basis(functions);
  return basis;
}

LinearModelStepwiseAlgorithm PolynomialRegressionAnalysis::buildAlgo(const OT::Sample & inputSample, const OT::Sample & outputSample)
{
  const Indices minimalIndices(1, 0);
  const Indices startIndices(1, 0);

  LinearModelStepwiseAlgorithm algo(inputSample, outputSample, getBasis(), minimalIndices, LinearModelStepwiseAlgorithm::BOTH, startIndices);
  algo.setPenalty(std::log(inputSample.getSize()));
  return algo;
}

void PolynomialRegressionAnalysis::computeAnalyticalValidation(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  if (stopRequested_)
    return;

  informationMessage_ = "The analytical validation is running.";
  notify("informationMessageUpdated");

  // retrieve chaos result
  PolynomialRegressionAnalysisResult lmResult(*dynamic_cast<PolynomialRegressionAnalysisResult*>(&result));

  // compute Q2
  Point q2(result.outputSample_.getDimension());
  const Point variance(result.outputSample_.computeVariance());
  const UnsignedInteger sampleSize = result.outputSample_.getSize();
  for (UnsignedInteger i = 0; i < result.outputSample_.getDimension(); ++i)
  {
    // get polynom basis
    Basis reducedBasis(lmResult.getLinearModelResultCollection()[i].getBasis());

    // compute basis matrix at the points of inputSample
    const UnsignedInteger basisSize = reducedBasis.getSize();
    Matrix A(inputSample.getSize(), basisSize);
    for (UnsignedInteger k = 0; k < basisSize; ++ k)
    {
      Sample outSample_k(reducedBasis[k](inputSample));
      for (UnsignedInteger j = 0; j < inputSample.getSize(); ++j)
        A(j, i) = outSample_k(j, 0);
    }

    // (A^t.A)
    CovarianceMatrix AtA(A.computeGram(true));

    // (A^t.A)^{-1}
    const Matrix AtA_inv(AtA.solveLinearSystem(IdentityMatrix(basisSize)).getImplementation());

    // A.(A^t.A)^{-1}.A^t
    const Matrix H(A * AtA_inv * A.transpose());

    Point Hdiag(H.getNbColumns());
    for (UnsignedInteger j = 0; j < H.getNbColumns(); ++ j)
      Hdiag[j] = H(j, j);

    // sum[ ((ŷ_j - y_j) / (1 - h_j))^2 ]
    Scalar quadraticResidual = 0.;
    for (UnsignedInteger j = 0; j < sampleSize; ++j)
    {
      const Scalar diff = (result.metaModelOutputSample_(j, i) - result.outputSample_(j, i)) / (1 - Hdiag[j]);
      quadraticResidual += diff * diff;
    }

    // 1 - sum[ ((ŷ_j - y_j) / (1 - h_j))^2 ] / (n-1) / Var Y
    q2[i] = 1.0 - (quadraticResidual / (sampleSize - 1.0)) / variance[i];
  }
  result.analyticalValidation_.q2_ = q2;
}

PolynomialRegressionAnalysisResult PolynomialRegressionAnalysis::getResult() const
{
  return result_;
}


Parameters PolynomialRegressionAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Linear model");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Interaction", getInteraction() ? "yes" : "no");
  param.add("Degree", getDegree());
  return param;
}


String PolynomialRegressionAnalysis::getPythonScript() const
{
  OSS oss;

  oss << getName() << " = persalys.PolynomialRegressionAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

  // interest outputs
  if (getInterestVariables().getSize() < getDesignOfExperiment().getOutputSample().getDimension())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  oss << getName() << ".setDegree(" << getDegree() << ")\n";
  oss << getName() << ".setInteraction(" << (getInteraction() ? "True" : "False") << ")\n";
  return oss;
}


bool PolynomialRegressionAnalysis::hasValidResult() const
{
  return getResult().getMetaModelOutputSample().getSize();
}


/* String converter */
String PolynomialRegressionAnalysis::__repr__() const
{
  OSS oss;
  oss << MetaModelAnalysis::__repr__()
      << " degree=" << getDegree()
      << " interaction=" << getInteraction();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void PolynomialRegressionAnalysis::save(Advocate& adv) const
{
  MetaModelAnalysis::save(adv);
  adv.saveAttribute("degree_", degree_);
  adv.saveAttribute("interaction_", interaction_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void PolynomialRegressionAnalysis::load(Advocate& adv)
{
  MetaModelAnalysis::load(adv);
  adv.loadAttribute("degree_", degree_);
  adv.loadAttribute("interaction_", interaction_);
  adv.loadAttribute("result_", result_);
}
}
