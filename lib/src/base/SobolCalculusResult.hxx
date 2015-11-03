// SobolCalculusResult.hxx

#ifndef SOBOLCALCULUSRESULT_H
#define SOBOLCALCULUSRESULT_H

#include "NumericalSample.hxx"

namespace OTGUI {
class SobolCalculusResult
{
public:
  SobolCalculusResult();
  SobolCalculusResult(const OT::NumericalSample firstOrderIndices,
                      const OT::NumericalSample totalOrderIndices,
                      const OT::Description & outputNames);

  virtual ~SobolCalculusResult(){};

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::NumericalSample getFirstOrderIndices() const;
  OT::NumericalSample getTotalOrderIndices() const;

private:
  OT::Description outputNames_;
  OT::Description inputNames_;
  OT::NumericalSample firstOrderIndices_;
  OT::NumericalSample totalOrderIndices_;
};
}
#endif