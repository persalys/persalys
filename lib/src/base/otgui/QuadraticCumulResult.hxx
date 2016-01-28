// QuadraticCumulResult.hxx

#ifndef QUADRATICCUMULRESULT_H
#define QUADRATICCUMULRESULT_H

#include "QuadraticCumul.hxx"
#include "Output.hxx"

namespace OTGUI {
class QuadraticCumulResult : public OT::PersistentObject
{
public:
  QuadraticCumulResult();
  QuadraticCumulResult(const OT::QuadraticCumul & algoQuadraticCumul, const OutputCollection & outputs);

  virtual QuadraticCumulResult * clone() const;

  OT::Description getOutputNames() const;
  OT::NumericalPoint getMeanFirstOrder() const;
  OT::NumericalPoint getMeanSecondOrder() const;
  OT::NumericalPoint getStandardDeviation();
  OT::NumericalPoint getVariance();

private:
  OT::QuadraticCumul algoQuadraticCumul_;
  OT::Description outputNames_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
};
}
#endif