//                                               -*- C++ -*-
/**
 *  @brief Inference analysis
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_INFERENCEANALYSIS_HXX
#define PERSALYS_INFERENCEANALYSIS_HXX

#include "DesignOfExperimentAnalysis.hxx"
#include "InferenceResult.hxx"

#include <openturns/DistributionFactory.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API InferenceAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME

public:
  typedef OT::Collection<OT::DistributionFactory> DistributionFactoryCollection;

  /** Default constructor */
  InferenceAnalysis();

  /** Constructor with parameters */
  InferenceAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);

  /** Virtual constructor */
  InferenceAnalysis * clone() const override;

  DistributionFactoryCollection getDistributionsFactories(const OT::String& variableName) const;
  void setDistributionsFactories(const OT::String& variableName, const DistributionFactoryCollection& distributionsFactories);

  double getLevel() const;
  void setLevel(const double level);

  void setInterestVariables(const OT::Description& variablesNames) override;

  InferenceResult getResult() const;

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

private:
  std::map<OT::String, DistributionFactoryCollection> distFactoriesForEachInterestVar_;
  double level_;
  InferenceResult result_;
};
}
#endif
