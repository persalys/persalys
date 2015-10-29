#include "SensitivityAnalysisItem.hxx"

namespace OTGUI {

SensitivityAnalysisItem::SensitivityAnalysisItem(const Calculus & calculus)
  : CalculusItem(calculus, QString("SensitivityAnalysis"))
{
}


void SensitivityAnalysisItem::update(Observable* source, const std::string & message)
{
  if (message=="calculusFinished")
  {
    emit calculusFinished(this);
  }
}

}