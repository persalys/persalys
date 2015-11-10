// SRCResult.hxx

#ifndef SRCRESULT_H
#define SRCRESULT_H

#include "NumericalSample.hxx"

namespace OTGUI {
class SRCResult
{
public:
  SRCResult();
  SRCResult(const OT::NumericalSample indices, const OT::Description & outputNames);

  virtual ~SRCResult(){};

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