//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_FIELDMONTECARLOANALYSIS_HXX
#define PERSALYS_FIELDMONTECARLOANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "FieldKarhunenLoeveAnalysis.hxx"

namespace PERSALYS
{

class PERSALYS_BASE_API FieldMonteCarloAnalysis : public SimulationAnalysis, public WithStopCriteriaAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  FieldMonteCarloAnalysis();
  /** Constructor with parameters */
  FieldMonteCarloAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  FieldMonteCarloAnalysis * clone() const override;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  /* Threshold accessors */
  OT::Scalar getKarhunenLoeveThreshold() const {return karhunenLoeveAnalysis_.getKarhunenLoeveThreshold();};
  void setKarhunenLoeveThreshold(const OT::Scalar threshold){karhunenLoeveAnalysis_.setKarhunenLoeveThreshold(threshold);};

  /* Quantile probability accessors */
  OT::Scalar getQuantileLevel() const {return karhunenLoeveAnalysis_.getQuantileLevel();};
  void setQuantileLevel(const OT::Scalar proba) {karhunenLoeveAnalysis_.setQuantileLevel(proba);};

  FieldMonteCarloResult getResult() const {return karhunenLoeveAnalysis_.getResult();};

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
  FieldKarhunenLoeveAnalysis karhunenLoeveAnalysis_;
};
}
#endif
