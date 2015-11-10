#include "otgui/CentralTendencyItem.hxx"

namespace OTGUI {

CentralTendencyItem::CentralTendencyItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("CentralTendency"))
{
}


void CentralTendencyItem::update(Observable* source, const std::string & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}

}