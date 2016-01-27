// ReliabilityAnalysisItem.hxx

#ifndef RELIABILITYANALYSISITEM_H
#define RELIABILITYANALYSISITEM_H

#include "otgui/AnalysisItem.hxx"

namespace OTGUI {
class ReliabilityAnalysisItem : public AnalysisItem
{
  Q_OBJECT

public:
  ReliabilityAnalysisItem(const Analysis & analysis);

  virtual void update(Observable * source, const OT::String & message);
};
}
#endif