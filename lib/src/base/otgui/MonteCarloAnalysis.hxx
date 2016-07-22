//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis 
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
#ifndef OTGUI_MONTECARLOANALYSIS_HXX
#define OTGUI_MONTECARLOANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "MonteCarloResult.hxx"

namespace OTGUI {
class OTGUI_API MonteCarloAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  /** Default constructor */
  MonteCarloAnalysis();
  /** Constructor with parameters */
  MonteCarloAnalysis(const OT::String & name, const PhysicalModel & physicalModel,
                     const OT::UnsignedInteger nbSimu=10000, bool confidenceInterval=true, double level=0.95);

  /** Virtual constructor */
  virtual MonteCarloAnalysis * clone() const;

  bool isConfidenceIntervalRequired() const;
  void setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired);

  double getLevelConfidenceInterval() const;
  void setLevelConfidenceInterval(const double levelConfidenceInterval);

  double getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const double coef);

  OT::UnsignedInteger getMaximumElapsedTime() const;
  void setMaximumElapsedTime(const OT::UnsignedInteger seconds);

  OT::UnsignedInteger getBlockSize() const;
  void setBlockSize(const OT::UnsignedInteger size);

  MonteCarloResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  double maximumCoefficientOfVariation_;
  OT::UnsignedInteger maximumElapsedTime_;
  OT::UnsignedInteger blockSize_;
  MonteCarloResult result_;
};
}
#endif