// SensitivityAnalysisItem.hxx

#ifndef SENSITIVITYANALYSISITEM_H
#define SENSITIVITYANALYSISITEM_H

#include "otgui/AnalysisItem.hxx"

namespace OTGUI {
class SensitivityAnalysisItem : public AnalysisItem
{
  Q_OBJECT

public:
  SensitivityAnalysisItem(const Analysis & analysis);

  virtual void update(Observable * source, const std::string & message);
};
}
#endif