//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all analysis with design of experiments
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTANALYSIS_HXX
#define PERSALYS_DESIGNOFEXPERIMENTANALYSIS_HXX

#include "AnalysisImplementation.hxx"
#include "DesignOfExperiment.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API DesignOfExperimentAnalysis : public AnalysisImplementation
{
public:
  /** Default constructor */
  DesignOfExperimentAnalysis();

  /** Constructor with parameters */
  DesignOfExperimentAnalysis(const OT::String & name);
  DesignOfExperimentAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment);

  /** Virtual constructor */
  DesignOfExperimentAnalysis * clone() const override;

  Observer * getParentObserver() const override;

  DesignOfExperiment getDesignOfExperiment() const;

  bool canBeLaunched(OT::String &errorMessage) const override;
  void run() override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

  void acceptLaunchParameters(LaunchParametersVisitor* visitor) override;

protected:
  DesignOfExperiment designOfExperiment_;
};
}
#endif
