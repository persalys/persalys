// SRCResult.cxx

#include "otgui/SRCResult.hxx"

using namespace OT;

namespace OTGUI {

SRCResult::SRCResult()
{
}


SRCResult::SRCResult(const NumericalSample indices,
                                     const Description & outputNames)
 : outputNames_(outputNames)
 , inputNames_(indices.getDescription())
 , indices_(indices)
{
}


Description SRCResult::getOutputNames() const
{
  return outputNames_;
}


Description SRCResult::getInputNames() const
{
  return inputNames_;
}


NumericalSample SRCResult::getIndices() const
{
  return indices_;
}

}