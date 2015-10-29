// SobolCalculusResult.hxx

#ifndef SOBOLCALCULUSRESULT_H
#define SOBOLCALCULUSRESULT_H

#include "NumericalPoint.hxx"

namespace OTGUI {
class SobolCalculusResult
{
public:
  SobolCalculusResult();
  SobolCalculusResult(const OT::NumericalPoint firstOrderIndices, const OT::NumericalPoint totalOrderIndices);

  SobolCalculusResult(const SobolCalculusResult & other);
  virtual SobolCalculusResult * clone() const;

  virtual ~SobolCalculusResult(){};

  OT::NumericalPoint getFirstOrderIndices() const;
  OT::NumericalPoint getTotalOrderIndices() const;

private:
  OT::NumericalPoint firstOrderIndices_;
  OT::NumericalPoint totalOrderIndices_;
};
}
#endif