//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMomentsAnalysis computes the moments with the Taylor Expansion method
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"

#include "QuadraticCumul.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(TaylorExpansionMomentsAnalysis);

static Factory<TaylorExpansionMomentsAnalysis> RegisteredFactory("TaylorExpansionMomentsAnalysis");

/* Default constructor */
TaylorExpansionMomentsAnalysis::TaylorExpansionMomentsAnalysis()
  : AnalysisImplementation()
{

}


/* Constructor with parameters */
TaylorExpansionMomentsAnalysis::TaylorExpansionMomentsAnalysis(const String & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
// TODO  , outputNames_(physicalModel.getOutputNames())
{
}


/* Virtual constructor */
TaylorExpansionMomentsAnalysis* TaylorExpansionMomentsAnalysis::clone() const
{
  return new TaylorExpansionMomentsAnalysis(*this);
}


// Description TaylorExpansionMomentsAnalysis::getOutputNames() const
// {
//   return outputNames_;
// }
// 
// 
// void TaylorExpansionMomentsAnalysis::setOutputNames(const Description & outputNames)
// {
//   outputNames_ = outputNames;
// }


void TaylorExpansionMomentsAnalysis::run()
{
  QuadraticCumul algoTaylorExpansionMoments(getPhysicalModel().getOutputRandomVector(getPhysicalModel().getOutputNames()));

  // set results
  NumericalPoint meanFirstOrder = algoTaylorExpansionMoments.getMeanFirstOrder();
  NumericalPoint meanSecondOrder = algoTaylorExpansionMoments.getMeanSecondOrder();
  NumericalPoint variance = NumericalPoint(algoTaylorExpansionMoments.getCovariance().getDimension());
  for (UnsignedInteger i=0; i<variance.getDimension(); ++i)
    variance[i] = algoTaylorExpansionMoments.getCovariance()(i,i);
  NumericalPoint standardDeviation(variance.getDimension());
  for (UnsignedInteger i=0; i<variance.getDimension(); ++i)
    standardDeviation[i] = sqrt(variance[i]);

  result_ = TaylorExpansionMomentsResult(getPhysicalModel().getOutputNames(), meanFirstOrder, meanSecondOrder, standardDeviation, variance);

  notify("analysisFinished");
}


TaylorExpansionMomentsResult TaylorExpansionMomentsAnalysis::getResult() const
{
  return result_;
}


String TaylorExpansionMomentsAnalysis::getPythonScript() const
{
  String result;
  result += getName()+ " = otguibase.TaylorExpansionMomentsAnalysis('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool TaylorExpansionMomentsAnalysis::analysisLaunched() const
{
  return getResult().getOutputNames().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void TaylorExpansionMomentsAnalysis::save(Advocate & adv) const
{
  AnalysisImplementation::save(adv);
//   adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void TaylorExpansionMomentsAnalysis::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
//   adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("result_", result_);
}
}