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
#include "otgui/SimulationAnalysisResult.hxx"

#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationAnalysisResult);

static Factory<SimulationAnalysisResult> RegisteredFactory("SimulationAnalysisResult");

/* Default constructor */
SimulationAnalysisResult::SimulationAnalysisResult()
  : ModelEvaluationResult()
{
}


/* Constructor with parameters */
SimulationAnalysisResult::SimulationAnalysisResult(NumericalSample inputSample, NumericalSample outputSample)
  : ModelEvaluationResult(inputSample, outputSample)
{
  searchMinMax();
}


/* Virtual constructor */
SimulationAnalysisResult* SimulationAnalysisResult::clone() const
{
  return new SimulationAnalysisResult(*this);
}


NumericalSampleCollection SimulationAnalysisResult::getListXMin() const
{
  return listXMin_;
}


NumericalSampleCollection SimulationAnalysisResult::getListXMax() const
{
  return listXMax_;
}


void SimulationAnalysisResult::searchMinMax()
{
  UnsignedInteger size = getInputSample().getSize();
  UnsignedInteger numberInputs = getInputSample().getDimension();

  Indices indicesInputs(numberInputs);
  indicesInputs.fill();

  NumericalSample sample = getInputSample();
  sample.stack(getOutputSample());

  for (UnsignedInteger i=numberInputs; i<sample.getDimension(); ++i)
  {
    NumericalSample orderedSample = sample.sortAccordingToAComponent(i);

    // Search min value of the ith output and the corresponding set of inputs X
    double minValue = orderedSample[0][i];

    UnsignedInteger it = 0;
    double value = orderedSample[it][i];
    NumericalSample tempSample(0, numberInputs);
    do
    {
      NumericalPoint point = orderedSample.getMarginal(indicesInputs)[it];
      if (!tempSample.__contains__(point))
        tempSample.add(point);
      ++it;
      value = orderedSample[it][i];
    } while (value == minValue && it < size);

    listXMin_.add(tempSample);

    // Search max value of the ith output and the corresponding set of inputs X
    double maxValue = orderedSample[size-1][i];

    it = 0;
    value = orderedSample[size-1-it][i];
    tempSample = NumericalSample(0, numberInputs);
    do
    {
      NumericalPoint point = orderedSample.getMarginal(indicesInputs)[size-1-it];
      if (!tempSample.__contains__(point))
        tempSample.add(point);
      ++it;
      value = orderedSample[size-1-it][i];
    } while (value == maxValue && it < size);

    listXMax_.add(tempSample);
  }
}


/* Method save() stores the object through the StorageManager */
void SimulationAnalysisResult::save(Advocate & adv) const
{
  ModelEvaluationResult::save(adv);
  adv.saveAttribute("listXMin_", listXMin_);
  adv.saveAttribute("listXMax_", listXMax_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationAnalysisResult::load(Advocate & adv)
{
  ModelEvaluationResult::load(adv);
  adv.loadAttribute("listXMin_", listXMin_);
  adv.loadAttribute("listXMax_", listXMax_);
}
}