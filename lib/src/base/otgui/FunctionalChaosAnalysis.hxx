//                                               -*- C++ -*-
/**
 *  @brief Class to define functional chaos
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_FUNCTIONALCHAOSANALYSIS_HXX
#define OTGUI_FUNCTIONALCHAOSANALYSIS_HXX

#include "MetaModelAnalysis.hxx"
#include "FunctionalChaosAnalysisResult.hxx"

#include <openturns/OrthogonalProductPolynomialFactory.hxx>
#include <openturns/FunctionalChaosAlgorithm.hxx>

namespace OTGUI
{
class OTGUI_API FunctionalChaosAnalysis : public MetaModelAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  FunctionalChaosAnalysis();
  /** Constructor with parameters */
  FunctionalChaosAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);
  FunctionalChaosAnalysis(const OT::String& name, const Analysis& analysis);

  /** Virtual constructor */
  virtual FunctionalChaosAnalysis * clone() const;

  OT::UnsignedInteger getChaosDegree() const;
  void setChaosDegree(const OT::UnsignedInteger degree);

  bool getSparseChaos() const;
  void setSparseChaos(const bool sparse);

  FunctionalChaosAnalysisResult getResult() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void initialize();
  virtual void launch();
  virtual OT::Function runAlgo(const OT::Sample& inputSample, const OT::Sample& outputSample);
  OT::FunctionalChaosAlgorithm buildFunctionalChaosAlgorithm(const OT::Sample & inputSample, const OT::Sample & outputSample);
  void postProcessFunctionalChaosResult(const OT::Sample& inputSample);
  OT::OrthogonalProductPolynomialFactory::PolynomialFamilyCollection getPolynomialFamilyCollection();
  virtual void computeAnalyticalValidation(MetaModelAnalysisResult& result, const OT::Sample& inputSample);

private:
  OT::OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomialFamilyCollection_;
  OT::UnsignedInteger chaosDegree_;
  bool sparseChaos_;
  FunctionalChaosAnalysisResult result_;
};
}
#endif
