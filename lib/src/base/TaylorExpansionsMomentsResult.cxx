//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionsMomentsResult contains the moments computed with the Taylor Expansion method
 *
 *  Copyright 2015-2016 EDF
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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