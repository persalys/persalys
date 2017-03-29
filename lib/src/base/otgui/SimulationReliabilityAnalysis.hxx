//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using simulation methods
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
#ifndef OTGUI_SIMULATIONRELIABILITYANALYSIS_HXX
#define OTGUI_SIMULATIONRELIABILITYANALYSIS_HXX

#include "ReliabilityAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "SimulationReliabilityResult.hxx"
#include "SimulationInterface.hxx"

#include <openturns/Simulation.hxx>

namespace OTGUI {
class OTGUI_API SimulationReliabilityAnalysis : public ReliabilityAnalysis, public WithStopCriteriaAnalysis
{
public:
  /** Default constructor */
  SimulationReliabilityAnalysis();
  /** Constructor with parameters */
  SimulationReliabilityAnalysis(const OT::String& name, const LimitState& limitState);

  /** Virtual constructor */
  virtual SimulationReliabilityAnalysis * clone() const;

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

  SimulationReliabilityResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  virtual void stop();

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual SimulationInterface getSimulationAlgorithm(const OT::Event& event);
  static void UpdateProgressValue(double percent, void* data);

  struct AnalysisStruct
  {
    AnalysisStruct(SimulationReliabilityAnalysis* analysis, SimulationInterface simulation)
    : analysis_(analysis)
    , simulation_(simulation)
    {
    };

    virtual ~AnalysisStruct(){};

    SimulationReliabilityAnalysis * analysis_;
    SimulationInterface simulation_;
  };

private:
  OT::UnsignedInteger seed_;
  TimeCriteria timeCriteria_;
protected:
  SimulationReliabilityResult result_;
};
}
#endif