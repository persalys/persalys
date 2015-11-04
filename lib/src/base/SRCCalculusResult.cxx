// SRCCalculusResult.cxx

#include "otgui/SRCCalculusResult.hxx"

using namespace OT;

namespace OTGUI {

SRCCalculusResult::SRCCalculusResult()
{
}


SRCCalculusResult::SRCCalculusResult(const NumericalSample indices,
                                     const Description & outputNames)
 : outputNames_(outputNames)
 , inputNames_(indices.getDescription())
 , indices_(indices)
{
}


Description SRCCalculusResult::getOutputNames() const
{
  return outputNames_;
}


Description SRCCalculusResult::getInputNames() const
{
  return inputNames_;
}


NumericalSample SRCCalculusResult::getIndices() const
{
  return indices_;
}

}