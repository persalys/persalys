//                                               -*- C++ -*-
/**
 *  @brief SimulationAnalysisResult contains results of a simulation analysis
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_SIMULATIONANALYSISRESULT_HXX
#define OTGUI_SIMULATIONANALYSISRESULT_HXX

#include "ModelEvaluationResult.hxx"

namespace OTGUI {
typedef OT::Collection<OT::NumericalSample> NumericalSampleCollection;
class OTGUI_API SimulationAnalysisResult : public ModelEvaluationResult
{
  CLASSNAME;

public:
  /** Default constructor */
  SimulationAnalysisResult();
  /** Constructor with parameters */
  SimulationAnalysisResult(OT::NumericalSample inputSample, OT::NumericalSample outputSample);

  /** Virtual constructor */
  virtual SimulationAnalysisResult * clone() const;

  NumericalSampleCollection getListXMin() const;
  NumericalSampleCollection getListXMax() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void searchMinMax();

private:
  OT::PersistentCollection<OT::NumericalSample> listXMin_;
  OT::PersistentCollection<OT::NumericalSample> listXMax_;
};
}
#endif