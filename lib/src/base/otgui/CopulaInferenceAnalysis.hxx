//                                               -*- C++ -*-
/**
 *  @brief Dependencies inference analysis
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
#ifndef OTGUI_COPULAINFERENCEANALYSIS_HXX
#define OTGUI_COPULAINFERENCEANALYSIS_HXX

#include "DesignOfExperimentAnalysis.hxx"
#include "CopulaInferenceResult.hxx"

#include <openturns/DistributionFactory.hxx>

namespace OTGUI {
class OTGUI_API CopulaInferenceAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME;

public:
  typedef OT::Collection<OT::DistributionFactory> DistributionFactoryCollection;

  /** Default constructor */
  CopulaInferenceAnalysis();

  /** Constructor with parameters */
  CopulaInferenceAnalysis(const OT::String& name, const DesignOfExperiment& designOfExperiment);

  /** Virtual constructor */
  virtual CopulaInferenceAnalysis * clone() const;

  DistributionFactoryCollection getDistributionsFactories(const OT::Description& variablesNames) const;
  void setDistributionsFactories(const OT::Description& variablesNames, const DistributionFactoryCollection& distributionsFactories);

  CopulaInferenceResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  std::map<OT::Description, DistributionFactoryCollection> distFactoriesForSetVar_;
  CopulaInferenceResult result_;
};
}
#endif