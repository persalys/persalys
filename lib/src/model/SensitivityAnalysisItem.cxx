#include "otgui/SensitivityAnalysisItem.hxx"

using namespace OT;

namespace OTGUI {

SensitivityAnalysisItem::SensitivityAnalysisItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("SensitivityAnalysis"))
{
}


void SensitivityAnalysisItem::update(Observable* source, const String & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}
}