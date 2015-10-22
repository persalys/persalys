// QuadraticCumulResult.cxx

#include "QuadraticCumulResult.hxx"

using namespace OT;

namespace OTGUI {

QuadraticCumulResult::QuadraticCumulResult()
{

}


QuadraticCumulResult::QuadraticCumulResult(const QuadraticCumul & algoQuadraticCumul, const OutputCollection & outputs)
 : algoQuadraticCumul_(algoQuadraticCumul)
{
  int nbOutputs = outputs.getSize();
  outputNames_ = Description(nbOutputs);
  for (int i=0; i<nbOutputs; ++i)
    outputNames_[i] = outputs[i].getName();
}


Description QuadraticCumulResult::getOutputNames() const
{
  return outputNames_;
}


NumericalPoint QuadraticCumulResult::getMeanFirstOrder() const
{
  return algoQuadraticCumul_.getMeanFirstOrder();
}


NumericalPoint QuadraticCumulResult::getMeanSecondOrder() const
{
  return algoQuadraticCumul_.getMeanSecondOrder();
}


NumericalPoint QuadraticCumulResult::getStandardDeviation()
{
  if (!standardDeviation_.getSize())
  {
    standardDeviation_ = NumericalPoint(getVariance().getDimension());
    for (int i=0; i<getVariance().getDimension(); ++i)
      standardDeviation_[i] = sqrt(getVariance()[i]);
  }
  return standardDeviation_;
}


NumericalPoint QuadraticCumulResult::getVariance()
{
  if (!variance_.getSize())
  {
    variance_ = NumericalPoint(algoQuadraticCumul_.getCovariance().getDimension());
    for (int i=0; i<variance_.getDimension(); ++i)
      variance_[i] = algoQuadraticCumul_.getCovariance()(i,i);
  }
  return variance_;
}

}