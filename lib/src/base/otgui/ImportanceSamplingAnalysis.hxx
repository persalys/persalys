//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the Importance sampling method
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_IMPORTANCESAMPLINGANALYSIS_HXX
#define OTGUI_IMPORTANCESAMPLINGANALYSIS_HXX

#include "SimulationReliabilityAnalysis.hxx"

namespace OTGUI
{
class OTGUI_API ImportanceSamplingAnalysis : public SimulationReliabilityAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  ImportanceSamplingAnalysis();
  /** Constructor with parameters */
  ImportanceSamplingAnalysis(const OT::String& name, const LimitState& limitState);

  /** Virtual constructor */
  virtual ImportanceSamplingAnalysis * clone() const;

  OT::Point getStandardSpaceDesignPoint() const;
  void setStandardSpaceDesignPoint(const OT::Point& point);

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual SimulationInterface getSimulationAlgorithm(const OT::Event& event);

private:
  OT::Point standardSpaceDesignPoint_;
};
}
#endif
