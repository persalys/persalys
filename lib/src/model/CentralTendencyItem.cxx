#include "otgui/CentralTendencyItem.hxx"

using namespace OT;

namespace OTGUI {

CentralTendencyItem::CentralTendencyItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("CentralTendency"))
{
}


void CentralTendencyItem::update(Observable* source, const String & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}
}