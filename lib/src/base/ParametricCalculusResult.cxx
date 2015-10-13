// ParametricCalculusResult.cxx

#include "ParametricCalculusResult.hxx"

namespace OTGUI {

CLASSNAMEINIT(ParametricCalculusResult);

ParametricCalculusResult::ParametricCalculusResult()
{

}


ParametricCalculusResult::ParametricCalculusResult(OT::NumericalSample resultSample, OT::NumericalSample inputSample)
 : resultSample_(resultSample)
 , listMin_(resultSample.getMin())
 , listMax_(resultSample.getMax())
{
  searchMinMax(inputSample);
}


ParametricCalculusResult::~ParametricCalculusResult()
{

}


OT::NumericalSample ParametricCalculusResult::getResultSample() const
{
  return resultSample_;
}


NumericalSampleCollection ParametricCalculusResult::getListXMin() const
{
  return listXMin_;
}


OT::NumericalPoint ParametricCalculusResult::getListMin() const
{
  return resultSample_.getMin();
}


NumericalSampleCollection ParametricCalculusResult::getListXMax() const
{
  return listXMax_;
}


OT::NumericalPoint ParametricCalculusResult::getListMax() const
{
  return resultSample_.getMax();
}


OT::Description ParametricCalculusResult::getOutputNames() const
{
  return resultSample_.getDescription();
}


void ParametricCalculusResult::searchMinMax(OT::NumericalSample inputSample)
{
  int size = inputSample.getSize();
  int numberInputs = inputSample.getDimension();

  OT::Indices indicesInputs(numberInputs);
  indicesInputs.fill();

  OT::NumericalSample sample = inputSample;
  sample.stack(resultSample_);

  for (int i=numberInputs; i<sample.getDimension(); ++i)
  {
    OT::NumericalSample orderedSample = sample.sortAccordingToAComponent(i);

    // Search min value of the ith output and the corresponding set of inputs X
    double minValue = orderedSample[0][i];

    int it=0;
    double value = orderedSample[it][i];
    OT::NumericalSample tempSample(0, numberInputs);
    do
    {
      OT::NumericalPoint point = orderedSample.getMarginal(indicesInputs)[it];
      if (!tempSample.__contains__(point))
        tempSample.add(point);
      ++it;
      value = orderedSample[it][i];
    } while (value == minValue && it < size);

    listXMin_.add(tempSample);

    // Search max value of the ith output and the corresponding set of inputs X
    double maxValue = orderedSample[size-1][i];

    it=0;
    value = orderedSample[size-1-it][i];
    tempSample = OT::NumericalSample(0, numberInputs);
    do
    {
      OT::NumericalPoint point = orderedSample.getMarginal(indicesInputs)[size-1-it];
      if (!tempSample.__contains__(point))
        tempSample.add(point);
      ++it;
      value = orderedSample[size-1-it][i];
    } while (value == maxValue && it < size);

    listXMax_.add(tempSample);
  }
}



}