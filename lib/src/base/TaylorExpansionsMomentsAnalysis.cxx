//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionsMomentsAnalysis computes the moments with the Taylor Expansion method
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
#include "otgui/TaylorExpansionsMomentsAnalysis.hxx"

#include "QuadraticCumul.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(TaylorExpansionsMomentsAnalysis);

TaylorExpansionsMomentsAnalysis::TaylorExpansionsMomentsAnalysis(const String & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
  , outputs_(physicalModel.getOutputs())
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


TaylorExpansionsMomentsAnalysis* TaylorExpansionsMomentsAnalysis::clone() const
{
  return new TaylorExpansionsMomentsAnalysis(*this);
}


OutputCollection TaylorExpansionsMomentsAnalysis::getOutputs() const
{
  return outputs_;
}


void TaylorExpansionsMomentsAnalysis::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


void TaylorExpansionsMomentsAnalysis::run()
{
//   TODO: cf CTR
  Description outputNames(outputs_.getSize());
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    outputNames[i] = outputs_[i].getName();

  QuadraticCumul algoTaylorExpansionsMoments(getPhysicalModel().getOutputRandomVector(outputNames));

  // set results
  result_ = TaylorExpansionsMomentsResult(algoTaylorExpansionsMoments, outputs_);

  notify("analysisFinished");
}


TaylorExpansionsMomentsResult TaylorExpansionsMomentsAnalysis::getResult() const
{
  return result_;
}


String TaylorExpansionsMomentsAnalysis::dump() const
{
  String result;
  result += getName()+ " = otguibase.TaylorExpansionsMomentsAnalysis('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool TaylorExpansionsMomentsAnalysis::analysisLaunched() const
{
//   return outputSample_.getSize()!=0;
}
}