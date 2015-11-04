// SRCCalculusResult.hxx

#ifndef SRCCALCULUSRESULT_H
#define SRCCALCULUSRESULT_H

#include "NumericalSample.hxx"

namespace OTGUI {
class SRCCalculusResult
{
public:
  SRCCalculusResult();
  SRCCalculusResult(const OT::NumericalSample indices,
                    const OT::Description & outputNames);

  virtual ~SRCCalculusResult(){};

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::NumericalSample getIndices() const;

private:
  OT::Description outputNames_;
  OT::Description inputNames_;
  OT::NumericalSample indices_;
};
}
#endif