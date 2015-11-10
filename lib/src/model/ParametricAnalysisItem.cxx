#include "otgui/ParametricAnalysisItem.hxx"

#include "otgui/ParametricAnalysis.hxx"

namespace OTGUI {

ParametricAnalysisItem::ParametricAnalysisItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("ParametricAnalysis"))
{
}


void ParametricAnalysisItem::update(Observable* source, const std::string & message)
{
  if (message=="inputSampleChanged")
  {
    emit inputSampleChanged(dynamic_cast<ParametricAnalysis*>(&*getAnalysis().getImplementation())->getInputSample());
  }
  else if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}

}