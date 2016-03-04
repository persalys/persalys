//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionsMomentsAnalysis computes the moments with the Taylor Expansion method
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
#include "otgui/TaylorExpansionsMomentsAnalysis.hxx"

#include "QuadraticCumul.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(TaylorExpansionsMomentsAnalysis);

static Factory<TaylorExpansionsMomentsAnalysis> RegisteredFactory("TaylorExpansionsMomentsAnalysis");

/* Default constructor */
TaylorExpansionsMomentsAnalysis::TaylorExpansionsMomentsAnalysis()
  : AnalysisImplementation()
{

}


/* Constructor with parameters */
TaylorExpansionsMomentsAnalysis::TaylorExpansionsMomentsAnalysis(const String & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
  , outputNames_(physicalModel.getOutputNames())
{
}


/* Virtual constructor */
TaylorExpansionsMomentsAnalysis* TaylorExpansionsMomentsAnalysis::clone() const
{
  return new TaylorExpansionsMomentsAnalysis(*this);
}


Description TaylorExpansionsMomentsAnalysis::getOutputNames() const
{
  return outputNames_;
}


void TaylorExpansionsMomentsAnalysis::setOutputNames(const Description & outputNames)
{
  outputNames_ = outputNames;
}


void TaylorExpansionsMomentsAnalysis::run()
{
  QuadraticCumul algoTaylorExpansionsMoments(getPhysicalModel().getOutputRandomVector(outputNames_));

  // set results
  NumericalPoint meanFirstOrder = algoTaylorExpansionsMoments.getMeanFirstOrder();
  NumericalPoint meanSecondOrder = algoTaylorExpansionsMoments.getMeanSecondOrder();
  NumericalPoint variance = NumericalPoint(algoTaylorExpansionsMoments.getCovariance().getDimension());
  for (UnsignedInteger i=0; i<variance.getDimension(); ++i)
    variance[i] = algoTaylorExpansionsMoments.getCovariance()(i,i);
  NumericalPoint standardDeviation(variance.getDimension());
  for (UnsignedInteger i=0; i<variance.getDimension(); ++i)
    standardDeviation[i] = sqrt(variance[i]);

  result_ = TaylorExpansionsMomentsResult(outputNames_, meanFirstOrder, meanSecondOrder, standardDeviation, variance);

  notify("analysisFinished");
}


TaylorExpansionsMomentsResult TaylorExpansionsMomentsAnalysis::getResult() const
{
  return result_;
}


String TaylorExpansionsMomentsAnalysis::getPythonScript() const
{
  String result;
  result += getName()+ " = otguibase.TaylorExpansionsMomentsAnalysis('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool TaylorExpansionsMomentsAnalysis::analysisLaunched() const
{
  return getResult().getOutputNames().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void TaylorExpansionsMomentsAnalysis::save(Advocate & adv) const
{
  AnalysisImplementation::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void TaylorExpansionsMomentsAnalysis::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("result_", result_);
}
}