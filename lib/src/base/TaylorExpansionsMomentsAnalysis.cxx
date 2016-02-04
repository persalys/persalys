// TaylorExpansionsMomentsAnalysis.cxx

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