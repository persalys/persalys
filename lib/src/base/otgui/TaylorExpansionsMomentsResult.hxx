// TaylorExpansionsMomentsResult.hxx

#ifndef OTGUI_TAYLOREXPANSIONSMOMENTSRESULT_H
#define OTGUI_TAYLOREXPANSIONSMOMENTSRESULT_H

#include "Output.hxx"

#include "QuadraticCumul.hxx"

namespace OTGUI {
class TaylorExpansionsMomentsResult : public OT::PersistentObject
{
public:
  TaylorExpansionsMomentsResult();
  TaylorExpansionsMomentsResult(const OT::QuadraticCumul & algoTaylorExpansionsMoments, const OutputCollection & outputs);

  virtual TaylorExpansionsMomentsResult * clone() const;

  OT::Description getOutputNames() const;
  OT::NumericalPoint getMeanFirstOrder() const;
  OT::NumericalPoint getMeanSecondOrder() const;
  OT::NumericalPoint getStandardDeviation();
  OT::NumericalPoint getVariance();

private:
  OT::QuadraticCumul algoTaylorExpansionsMoments_;
  OT::Description outputNames_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
};
}
#endif