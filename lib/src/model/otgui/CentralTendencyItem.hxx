// CentralTendencyItem.hxx

#ifndef DISTRIBUTIONANALYSISITEM_H
#define DISTRIBUTIONANALYSISITEM_H

#include "AnalysisItem.hxx"

namespace OTGUI {
class CentralTendencyItem : public AnalysisItem
{
  Q_OBJECT

public:
  CentralTendencyItem(const Analysis & analysis);

  virtual void update(Observable * source, const OT::String & message);
};
}
#endif