// SimulationAnalysisResult.cxx

#include "otgui/SimulationAnalysisResult.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationAnalysisResult);

SimulationAnalysisResult::SimulationAnalysisResult()
  : ModelEvaluationResult()
{

}


SimulationAnalysisResult::SimulationAnalysisResult(NumericalSample inputSample, NumericalSample outputSample)
  : ModelEvaluationResult(inputSample, outputSample)
{
  searchMinMax();
}


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
  int size = getInputSample().getSize();
  int numberInputs = getInputSample().getDimension();

  Indices indicesInputs(numberInputs);
  indicesInputs.fill();

  NumericalSample sample = getInputSample();
  sample.stack(getOutputSample());

  for (int i=numberInputs; i<sample.getDimension(); ++i)
  {
    NumericalSample orderedSample = sample.sortAccordingToAComponent(i);

    // Search min value of the ith output and the corresponding set of inputs X
    double minValue = orderedSample[0][i];

    int it = 0;
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
}