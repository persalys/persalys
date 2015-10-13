// QuadraticCumulResult.hxx

#ifndef QUADRATICCUMULRESULT_H
#define QUADRATICCUMULRESULT_H

#include "NumericalPoint.hxx"
#include "QuadraticCumul.hxx"

namespace OTGUI {
class QuadraticCumulResult
{
public:
  QuadraticCumulResult(){};
  QuadraticCumulResult(const OT::QuadraticCumul & algoQuadraticCumul)
  {
    firstOrderMean_ = algoQuadraticCumul.getMeanFirstOrder();
    secondOrderMean_ = algoQuadraticCumul.getMeanSecondOrder();
    variance_ = OT::NumericalPoint(algoQuadraticCumul.getCovariance().getDimension());
    standardDeviation_ = OT::NumericalPoint(algoQuadraticCumul.getCovariance().getDimension());
    for (unsigned int i = 0; i < algoQuadraticCumul.getCovariance().getDimension(); ++i)
    {
      variance_[i] = algoQuadraticCumul.getCovariance()(i,i);
      standardDeviation_[i] = sqrt(variance_[i]);
    }
  };
  virtual ~QuadraticCumulResult(){};

  OT::NumericalPoint getFirstOrderMean() const {return firstOrderMean_;};
  OT::NumericalPoint getSecondOrderMean() const {return secondOrderMean_;};
  OT::NumericalPoint getStandardDeviation() const {return standardDeviation_;};
  OT::NumericalPoint getVariance() const {return variance_;};

private:
  OT::NumericalPoint firstOrderMean_;
  OT::NumericalPoint secondOrderMean_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
};
}
#endif