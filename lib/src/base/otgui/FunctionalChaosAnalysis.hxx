//                                               -*- C++ -*-
/**
 *  @brief Class to define functional chaos
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
#ifndef OTGUI_FUNCTIONALCHAOSANALYSIS_HXX
#define OTGUI_FUNCTIONALCHAOSANALYSIS_HXX

#include "DesignOfExperimentAnalysis.hxx"
#include "FunctionalChaosAnalysisResult.hxx"

#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/FunctionalChaosAlgorithm.hxx"

namespace OTGUI {
class OTGUI_API FunctionalChaosAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME;

public:
  /** Default constructor */
  FunctionalChaosAnalysis();
  /** Constructor with parameters */
  FunctionalChaosAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment);

  /** Virtual constructor */
  virtual FunctionalChaosAnalysis * clone() const;

  virtual void setDesignOfExperiment(const DesignOfExperiment & designOfExperiment);

  OT::UnsignedInteger getChaosDegree() const;
  void setChaosDegree(const OT::UnsignedInteger degree);

  bool getSparseChaos() const;
  void setSparseChaos(const bool sparse);

  bool isLeaveOneOutValidation() const;
  void setLeaveOneOutValidation(const bool validation);

  OT::ComposedDistribution getDistribution();

  FunctionalChaosAnalysisResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  // TODO remove it when OT version 1.8
  // Binomial coefficient
  static OT::UnsignedInteger BinomialCoefficient(const OT::UnsignedInteger n,
                                                 const OT::UnsignedInteger k);
private:
  OT::FunctionalChaosAlgorithm buildFunctionalChaosAlgorithm(const OT::NumericalSample & inputSample, const OT::NumericalSample & outputSample);
  void postProcessFunctionalChaosResult();
  OT::OrthogonalProductPolynomialFactory::PolynomialFamilyCollection getPolynomialFamilyCollection();
  void computeErrorQ2LOO();

private:
  OT::ComposedDistribution distribution_;
  bool isDistributionComputed_;
  OT::OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomialFamilyCollection_;
  OT::UnsignedInteger chaosDegree_;
  bool sparseChaos_;
  bool leaveOneOutValidation_;
  FunctionalChaosAnalysisResult result_;
};
}
#endif