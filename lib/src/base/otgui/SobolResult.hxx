// SobolResult.hxx

#ifndef SOBOLRESULT_H
#define SOBOLRESULT_H

#include "NumericalSample.hxx"

namespace OTGUI {
class SobolResult
{
public:
  SobolResult();
  SobolResult(const OT::NumericalSample firstOrderIndices,
              const OT::NumericalSample totalOrderIndices,
              const OT::Description & outputNames);

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::NumericalSample getFirstOrderIndices() const;
  OT::NumericalSample getTotalOrderIndices() const;

private:
  OT::Description outputNames_;
  OT::NumericalSample firstOrderIndices_;
  OT::NumericalSample totalOrderIndices_;
};
}
#endif