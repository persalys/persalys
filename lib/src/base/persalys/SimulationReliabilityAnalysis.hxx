//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using simulation methods
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
#ifndef PERSALYS_SIMULATIONRELIABILITYANALYSIS_HXX
#define PERSALYS_SIMULATIONRELIABILITYANALYSIS_HXX

#include "ReliabilityAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "SimulationReliabilityResult.hxx"
#include "SimulationInterface.hxx"

#include <openturns/SimulationAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API SimulationReliabilityAnalysis : public ReliabilityAnalysis, public WithStopCriteriaAnalysis
{
public:
  /** Default constructor */
  SimulationReliabilityAnalysis();
  /** Constructor with parameters */
  SimulationReliabilityAnalysis(const OT::String& name, const LimitState& limitState);

  /** Virtual constructor */
  SimulationReliabilityAnalysis * clone() const override;

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

  OT::UnsignedInteger getBlockSize() const;
  virtual void setBlockSize(const OT::UnsignedInteger size);

  Parameters getParameters() const override;

  SimulationReliabilityResult getResult() const;

  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  void stop() override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initialize() override;
  void launch() override;
  virtual SimulationInterface getSimulationAlgorithm(const OT::RandomVector& event);
  static void UpdateProgressValue(double percent, void* data);

private:
  OT::UnsignedInteger seed_;
  OT::UnsignedInteger blockSize_;
  TimeCriteria timeCriteria_;
protected:
  SimulationReliabilityResult result_;
};
}
#endif
