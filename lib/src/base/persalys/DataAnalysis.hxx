//                                               -*- C++ -*-
/**
 *  @brief Data analysis
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_DATAANALYSIS_HXX
#define PERSALYS_DATAANALYSIS_HXX

#include "DesignOfExperimentAnalysis.hxx"
#include "DataAnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API DataAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  DataAnalysis();

  /** Constructor with parameters */
  DataAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment);

  /** Virtual constructor */
  DataAnalysis * clone() const override;

  bool isConfidenceIntervalRequired() const;
  void setIsConfidenceIntervalRequired(const bool isRequired);

  double getLevelConfidenceInterval() const;
  void setLevelConfidenceInterval(const double level);

  DataAnalysisResult getResult() const;

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
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  DataAnalysisResult result_;
};
}
#endif
