// SobolCalculusResult.cxx

#include "SobolCalculusResult.hxx"

using namespace OT;

namespace OTGUI {

SobolCalculusResult::SobolCalculusResult()
{
}


SobolCalculusResult::SobolCalculusResult(const NumericalSample firstOrderIndices,
                                         const NumericalSample totalOrderIndices,
                                         const Description & outputNames)
 : outputNames_(outputNames)
 , inputNames_(firstOrderIndices.getDescription())
 , firstOrderIndices_(firstOrderIndices)
 , totalOrderIndices_(totalOrderIndices)
{
}


Description SobolCalculusResult::getOutputNames() const
{
  return outputNames_;
}


Description SobolCalculusResult::getInputNames() const
{
  return inputNames_;
}


NumericalSample SobolCalculusResult::getFirstOrderIndices() const
{
  return firstOrderIndices_;
}


NumericalSample SobolCalculusResult::getTotalOrderIndices() const
{
  return totalOrderIndices_;
}
}