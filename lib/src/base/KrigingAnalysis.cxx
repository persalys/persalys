//                                               -*- C++ -*-
/**
 *  @brief Class to define kriging
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
#include "otgui/KrigingAnalysis.hxx"

#include "openturns/OTBase.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(KrigingAnalysis);

static Factory<KrigingAnalysis> RegisteredFactory;

/* Default constructor */
KrigingAnalysis::KrigingAnalysis()
  : MetaModelAnalysis()
{

}


/* Constructor with parameters */
KrigingAnalysis::KrigingAnalysis(const String& name, const DesignOfExperiment& designOfExperiment)
  : MetaModelAnalysis(name, designOfExperiment)
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
  if (getEffectiveInputSample().getDimension() != basis.getDimension())
    throw InvalidArgumentException(HERE) << "The basis dimension must be equal to the number of effective inputs " << getEffectiveInputSample().getDimension();

  basis_ = basis;
}


CovarianceModel KrigingAnalysis::getCovarianceModel() const
{
  return covarianceModel_;
}


void KrigingAnalysis::setCovarianceModel(const CovarianceModel& model)
{
  const UnsignedInteger inputDimension = getEffectiveInputSample().getDimension();
  if (model.getSpatialDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "The covariance model spatial dimension must be equal to the number of effective inputs " << inputDimension;
  if (model.getDimension() != 1)
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


void KrigingAnalysis::run()
{
  try
  {
    // clear result
    initialize();
    result_ = KrigingAnalysisResult();
    optimalCovarianceModel_ = CovarianceModel();

    // check
    if (designOfExperiment_.getInputSample().getSize()*designOfExperiment_.getOutputSample().getSize() == 0)
      throw InvalidArgumentException(HERE) << "The design of experiment must contains not empty input AND output samples";
    if (designOfExperiment_.getInputSample().getSize() != designOfExperiment_.getOutputSample().getSize())
      throw InvalidArgumentException(HERE) << "The input sample and the output sample must have the same size";
    if (!getInterestVariables().getSize())
      throw InvalidDimensionException(HERE) << "The number of outputs to analyse must be superior to 0";

    // get effective samples
    const NumericalSample effectiveInputSample(getEffectiveInputSample());
    const NumericalSample effectiveOutputSample(getEffectiveOutputSample());
    const UnsignedInteger size = effectiveInputSample.getSize();
    const UnsignedInteger outputDimension = effectiveOutputSample.getDimension();

    // Kriging
    Collection<KrigingResult> krigingResultCollection;
    NumericalMathFunction::NumericalMathFunctionCollection metaModelCollection;

    // temp
    NumericalSample metaModelLOO(size, outputDimension);
    NumericalPoint q2LOO(outputDimension);
    NumericalPoint errorQ2LOO(outputDimension);

    // for each output:
    // because with all outputs KrigingAlgorithm is not reliable
    for (UnsignedInteger i=0; i<outputDimension; ++i)
    {
      if (stopRequested_)
       break;

      // build algo
      KrigingAlgorithm kriging(buildKrigingAlgorithm(effectiveInputSample, effectiveOutputSample.getMarginal(i)));

      // run algo
      kriging.run();

      // get results
      krigingResultCollection.add(kriging.getResult());
      metaModelCollection.add(kriging.getResult().getMetaModel());
      optimalCovarianceModel_ = kriging.getResult().getCovarianceModel();

      // validation
      if (leaveOneOutValidation_)
      {
        MetaModelAnalysisResult result_i;
        result_i.outputSample_ = effectiveOutputSample.getMarginal(i);
        validateMetaModelResult(result_i, effectiveInputSample);
        for (UnsignedInteger j=0; j<size; ++j)
          metaModelLOO[j][i] = result_i.metaModelOutputSampleLOO_[j][0];

        q2LOO[i] = result_i.q2LOO_[0];
        errorQ2LOO[i] = result_i.errorQ2LOO_[0];
      }
    }

    // set result_
    Indices computedOutputIndices(krigingResultCollection.getSize());
    computedOutputIndices.fill();
    result_.outputSample_ = effectiveOutputSample.getMarginal(computedOutputIndices);
    result_.krigingResultCollection_ = krigingResultCollection;

    // build metamodel
    NumericalMathFunction metamodelFunction(metaModelCollection);
    Description variablesNames(effectiveInputSample.getDescription());
    variablesNames.add(result_.outputSample_.getDescription());
    metamodelFunction.setDescription(variablesNames);

    buildMetaModel(result_, metamodelFunction);
    result_.metaModelOutputSample_ = metamodelFunction(effectiveInputSample);

    // validation
    if (leaveOneOutValidation_)
    {
      result_.metaModelOutputSampleLOO_ = metaModelLOO;
      result_.q2LOO_ = q2LOO;
      result_.errorQ2LOO_ = errorQ2LOO;
    }

    notify("metaModelCreated");
    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
}


NumericalMathFunction KrigingAnalysis::runAlgo(const NumericalSample& inputSample, const NumericalSample& outputSample)
{
  if (outputSample.getDimension() > 1)
    throw InternalException(HERE) << "KrigingAnalysis::runAlgo: the output sample must have a dimension of 1";

  KrigingAlgorithm kriging(buildKrigingAlgorithm(inputSample, outputSample, true));
  kriging.setOptimizeParameters(false);
  kriging.run();

  return kriging.getResult().getMetaModel();
}


KrigingAlgorithm KrigingAnalysis::buildKrigingAlgorithm(const NumericalSample& inputSample,
                                                        const NumericalSample& outputSample,
                                                        const bool useOptimalCovModel)
{
  if (outputSample.getDimension() != 1)
    throw InternalException(HERE) << "KrigingAnalysis::buildKrigingAlgorithm: the output sample must have a dimension of 1";
  if (useOptimalCovModel && (optimalCovarianceModel_.getDimension() > 1))
    throw InternalException(HERE) << "KrigingAnalysis::buildKrigingAlgorithm: the optimal covariance model must have a dimension of 1";

  KrigingAlgorithm algo(inputSample,
                        getDistribution().getIsoProbabilisticTransformation(),
                        outputSample,
                        getBasis(),
                        useOptimalCovModel? optimalCovarianceModel_ : covarianceModel_);

  algo.setOptimizeParameters(optimizeParameters_);

  return algo;
}


KrigingAnalysisResult KrigingAnalysis::getResult() const
{
  return result_;
}


String KrigingAnalysis::getPythonScript() const
{
  OSS oss;

  oss << getName() << " = otguibase.KrigingAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

  // interest outputs
  if (getInterestVariables().getSize() < getDesignOfExperiment().getOutputSample().getDimension())
  {
    oss << "interestVariables = [";
    for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
    {
      oss << "'" << getInterestVariables()[i] << "'";
      if (i < getInterestVariables().getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  // basis
  const UnsignedInteger effectiveInputDim = getCovarianceModel().getScale().getSize();
  const UnsignedInteger basisDim = getBasis().getDimension();

  oss << getName() << ".setBasis(";
  if (getBasis().getSize() == (basisDim+1))
    oss << "ot.LinearBasisFactory(" << effectiveInputDim << ").build()";
  else if (getBasis().getSize() == ((basisDim+1)*(basisDim+2)/2))
    oss << "ot.QuadraticBasisFactory(" << effectiveInputDim << ").build()";
  else
    oss << "ot.ConstantBasisFactory(" << effectiveInputDim << ").build()";
  oss << ")\n";

  // covariance model
  oss << getName() << ".setCovarianceModel(ot." << getCovarianceModel().getImplementation()->getClassName()
      << "(" << getCovarianceModel().getScale().__str__() << ", "
      << getCovarianceModel().getAmplitude().__str__();

  if (getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
    oss << ", " << dynamic_cast<MaternModel*>(&*getCovarianceModel().getImplementation())->getNu();
  else if (getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
    oss << ", " << dynamic_cast<GeneralizedExponential*>(&*getCovarianceModel().getImplementation())->getP();
  oss << "))\n";

  oss << getName() << ".setOptimizeParameters(" <<(getOptimizeParameters()? "True" : "False") << ")\n";

  // validation
  oss << getName() << ".setLeaveOneOutValidation(" << (isLeaveOneOutValidation()? "True" : "False") << ")\n";

  return oss;
}


bool KrigingAnalysis::analysisLaunched() const
{
  return result_.outputSample_.getSize();
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