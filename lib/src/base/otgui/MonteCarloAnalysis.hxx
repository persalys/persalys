//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
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
#ifndef OTGUI_MONTECARLOANALYSIS_HXX
#define OTGUI_MONTECARLOANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "DataAnalysisResult.hxx"

namespace OTGUI
{
class OTGUI_API MonteCarloAnalysis : public SimulationAnalysis, public WithStopCriteriaAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  MonteCarloAnalysis();
  /** Constructor with parameters */
  MonteCarloAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual MonteCarloAnalysis * clone() const;

  bool isConfidenceIntervalRequired() const;
  void setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired);

  double getLevelConfidenceInterval() const;
  void setLevelConfidenceInterval(const double levelConfidenceInterval);

  DataAnalysisResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  DataAnalysisResult result_;
};
}
#endif
