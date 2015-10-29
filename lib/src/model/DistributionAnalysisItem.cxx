#include "DistributionAnalysisItem.hxx"

namespace OTGUI {

DistributionAnalysisItem::DistributionAnalysisItem(const Calculus & calculus)
  : CalculusItem(calculus, QString("DistributionAnalysis"))
{
}


void DistributionAnalysisItem::update(Observable* source, const std::string & message)
{
  if (message=="calculusFinished")
  {
    emit calculusFinished(this);
  }
}

}