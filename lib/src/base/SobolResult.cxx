// SobolResult.cxx

#include "otgui/SobolResult.hxx"

using namespace OT;

namespace OTGUI {

SobolResult::SobolResult()
{
}


SobolResult::SobolResult(const NumericalSample firstOrderIndices,
                         const NumericalSample totalOrderIndices,
                         const Description & outputNames)
  : outputNames_(outputNames)
  , firstOrderIndices_(firstOrderIndices)
  , totalOrderIndices_(totalOrderIndices)
{
}


SobolResult* SobolResult::clone() const
{
  return new SobolResult(*this);
}


Description SobolResult::getOutputNames() const
{
  return outputNames_;
}


Description SobolResult::getInputNames() const
{
  return firstOrderIndices_.getDescription();
}


NumericalSample SobolResult::getFirstOrderIndices() const
{
  return firstOrderIndices_;
}


NumericalSample SobolResult::getTotalOrderIndices() const
{
  return totalOrderIndices_;
}
}