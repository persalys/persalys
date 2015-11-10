// ParametricAnalysisItem.hxx

#ifndef PARAMETRICANALYSISITEM_H
#define PARAMETRICANALYSISITEM_H

#include "AnalysisItem.hxx"

namespace OTGUI {
class ParametricAnalysisItem : public AnalysisItem
{
  Q_OBJECT

public:
  ParametricAnalysisItem(const Analysis & analysis);

  virtual void update(Observable * source, const std::string & message);

signals:
  void inputSampleChanged(OT::NumericalSample);
};
}
#endif