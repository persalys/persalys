//                                               -*- C++ -*-
/**
 *  @brief Class to define functional chaos
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_FUNCTIONALCHAOSANALYSIS_HXX
#define PERSALYS_FUNCTIONALCHAOSANALYSIS_HXX

#include "MetaModelAnalysis.hxx"
#include "FunctionalChaosAnalysisResult.hxx"

#include <openturns/OrthogonalProductPolynomialFactory.hxx>
#include <openturns/FunctionalChaosAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API FunctionalChaosAnalysis : public MetaModelAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  FunctionalChaosAnalysis();
  /** Constructor with parameters */
  FunctionalChaosAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);
  FunctionalChaosAnalysis(const OT::String& name, const Analysis& analysis);

  /** Virtual constructor */
  FunctionalChaosAnalysis * clone() const override;

  OT::UnsignedInteger getChaosDegree() const;
  void setChaosDegree(const OT::UnsignedInteger degree);

  bool getSparseChaos() const;
  void setSparseChaos(const bool sparse);

  OT::Distribution getDistribution();

  FunctionalChaosAnalysisResult getResult() const;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initialize() override;
  void launch() override;
  OT::Function runAlgo(const OT::Sample& inputSample, const OT::Sample& outputSample) override;
  OT::FunctionalChaosAlgorithm buildFunctionalChaosAlgorithm(const OT::Sample & inputSample, const OT::Sample & outputSample);
  void postProcessFunctionalChaosResult(const OT::Sample& inputSample);
  OT::OrthogonalProductPolynomialFactory::PolynomialFamilyCollection getPolynomialFamilyCollection();
  void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample) override;

private:
  OT::OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomialFamilyCollection_;
  OT::UnsignedInteger chaosDegree_;
  bool sparseChaos_;
  FunctionalChaosAnalysisResult result_;
};
}
#endif
