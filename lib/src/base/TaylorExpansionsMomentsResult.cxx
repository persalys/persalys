// TaylorExpansionsMomentsResult.cxx

#include "otgui/TaylorExpansionsMomentsResult.hxx"

using namespace OT;

namespace OTGUI {

TaylorExpansionsMomentsResult::TaylorExpansionsMomentsResult()
{

}


TaylorExpansionsMomentsResult::TaylorExpansionsMomentsResult(const QuadraticCumul & algoTaylorExpansionsMoments, const OutputCollection & outputs)
 : algoTaylorExpansionsMoments_(algoTaylorExpansionsMoments)
{
  UnsignedInteger nbOutputs = outputs.getSize();
  outputNames_ = Description(nbOutputs);
  for (UnsignedInteger i=0; i<nbOutputs; ++i)
    outputNames_[i] = outputs[i].getName();
}


TaylorExpansionsMomentsResult* TaylorExpansionsMomentsResult::clone() const
{
  return new TaylorExpansionsMomentsResult(*this);
}


Description TaylorExpansionsMomentsResult::getOutputNames() const
{
  return outputNames_;
}


NumericalPoint TaylorExpansionsMomentsResult::getMeanFirstOrder() const
{
  return algoTaylorExpansionsMoments_.getMeanFirstOrder();
}


NumericalPoint TaylorExpansionsMomentsResult::getMeanSecondOrder() const
{
  return algoTaylorExpansionsMoments_.getMeanSecondOrder();
}


NumericalPoint TaylorExpansionsMomentsResult::getStandardDeviation()
{
  if (!standardDeviation_.getSize())
  {
    standardDeviation_ = NumericalPoint(getVariance().getDimension());
    for (UnsignedInteger i=0; i<getVariance().getDimension(); ++i)
      standardDeviation_[i] = sqrt(getVariance()[i]);
  }
  return standardDeviation_;
}


NumericalPoint TaylorExpansionsMomentsResult::getVariance()
{
  if (!variance_.getSize())
  {
    variance_ = NumericalPoint(algoTaylorExpansionsMoments_.getCovariance().getDimension());
    for (UnsignedInteger i=0; i<variance_.getDimension(); ++i)
      variance_[i] = algoTaylorExpansionsMoments_.getCovariance()(i,i);
  }
  return variance_;
}

}