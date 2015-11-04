// DistributionAnalysisItem.hxx

#ifndef DISTRIBUTIONANALYSISITEM_H
#define DISTRIBUTIONANALYSISITEM_H

#include "CalculusItem.hxx"

namespace OTGUI {
class DistributionAnalysisItem : public CalculusItem
{
  Q_OBJECT

public:
  DistributionAnalysisItem(const Calculus & calculus);

  virtual void update(Observable * source, const std::string & message);
};
}
#endif