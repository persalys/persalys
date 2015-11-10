#include "otgui/SensitivityAnalysisItem.hxx"

namespace OTGUI {

SensitivityAnalysisItem::SensitivityAnalysisItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("SensitivityAnalysis"))
{
}


void SensitivityAnalysisItem::update(Observable* source, const std::string & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}

}