#include "otgui/ReliabilityAnalysisItem.hxx"

namespace OTGUI {

ReliabilityAnalysisItem::ReliabilityAnalysisItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("ReliabilityAnalysis"))
{
}


void ReliabilityAnalysisItem::update(Observable* source, const std::string & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}

}