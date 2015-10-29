// SensitivityAnalysisItem.hxx

#ifndef SENSITIVITYANALYSISITEM_H
#define SENSITIVITYANALYSISITEM_H

#include "CalculusItem.hxx"

namespace OTGUI {
class SensitivityAnalysisItem : public CalculusItem
{
  Q_OBJECT

public:
  SensitivityAnalysisItem(const Calculus & calculus);

  virtual void update(Observable * source, const std::string & message);
};
}
#endif