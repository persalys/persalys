#include "otgui/ReliabilityAnalysisItem.hxx"

using namespace OT;

namespace OTGUI {

ReliabilityAnalysisItem::ReliabilityAnalysisItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("ReliabilityAnalysis"))
{
}


void ReliabilityAnalysisItem::update(Observable* source, const String & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}
}