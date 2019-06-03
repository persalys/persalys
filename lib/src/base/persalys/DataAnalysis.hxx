//                                               -*- C++ -*-
/**
 *  @brief Data analysis
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
class PERSALYS_API DataAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  DataAnalysis();

  /** Constructor with parameters */
  DataAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment);

  /** Virtual constructor */
  virtual DataAnalysis * clone() const;

  bool isConfidenceIntervalRequired() const;
  void setIsConfidenceIntervalRequired(const bool isRequired);

  double getLevelConfidenceInterval() const;
  void setLevelConfidenceInterval(const double level);

  DataAnalysisResult getResult() const;

  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void initialize();
  virtual void launch();

private:
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  DataAnalysisResult result_;
};
}
#endif
