// ParametricAnalysisResult.cxx

#include "otgui/ParametricAnalysisResult.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ParametricAnalysisResult);

ParametricAnalysisResult::ParametricAnalysisResult()
{

}


ParametricAnalysisResult::ParametricAnalysisResult(NumericalSample inputSample, NumericalSample resultSample)
  : resultSample_(resultSample)
  , inputSample_(inputSample)
  , listMin_(resultSample.getMin())
  , listMax_(resultSample.getMax())
{
  searchMinMax();
}


ParametricAnalysisResult::~ParametricAnalysisResult()
{

}


NumericalSample ParametricAnalysisResult::getResultSample() const
{
  return resultSample_;
}


NumericalSample ParametricAnalysisResult::getInputSample() const
{
  return inputSample_;
}


NumericalSampleCollection ParametricAnalysisResult::getListXMin() const
{
  return listXMin_;
}


NumericalPoint ParametricAnalysisResult::getListMin() const
{
  return resultSample_.getMin();
}


NumericalSampleCollection ParametricAnalysisResult::getListXMax() const
{
  return listXMax_;
}


NumericalPoint ParametricAnalysisResult::getListMax() const
{
  return resultSample_.getMax();
}


Description ParametricAnalysisResult::getOutputNames() const
{
  return resultSample_.getDescription();
}


Description ParametricAnalysisResult::getInputNames() const
{
  return inputSample_.getDescription();
}


void ParametricAnalysisResult::searchMinMax()
{
  int size = inputSample_.getSize();
  int numberInputs = inputSample_.getDimension();

  Indices indicesInputs(numberInputs);
  indicesInputs.fill();

  NumericalSample sample = inputSample_;
  sample.stack(resultSample_);

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