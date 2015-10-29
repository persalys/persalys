// SobolCalculusResult.cxx

#include "SobolCalculusResult.hxx"

using namespace OT;

namespace OTGUI {

SobolCalculusResult::SobolCalculusResult()
{
}


SobolCalculusResult::SobolCalculusResult(const NumericalPoint firstOrderIndices, const NumericalPoint totalOrderIndices)
 : firstOrderIndices_(firstOrderIndices), totalOrderIndices_(totalOrderIndices)
{

}


SobolCalculusResult::SobolCalculusResult(const SobolCalculusResult & other)
 : firstOrderIndices_(other.firstOrderIndices_)
 , totalOrderIndices_(other.totalOrderIndices_)
{
}


SobolCalculusResult * SobolCalculusResult::clone() const
{
  return new SobolCalculusResult(*this);
}


NumericalPoint SobolCalculusResult::getFirstOrderIndices() const
{
  return firstOrderIndices_;
}


NumericalPoint SobolCalculusResult::getTotalOrderIndices() const
{
  return totalOrderIndices_;
}
}