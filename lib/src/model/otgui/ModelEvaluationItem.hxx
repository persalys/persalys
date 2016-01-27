// ModelEvaluationItem.hxx

#ifndef MODELEVALUATIONSITEM_H
#define MODELEVALUATIONSITEM_H

#include "AnalysisItem.hxx"

namespace OTGUI {
class ModelEvaluationItem : public AnalysisItem
{
  Q_OBJECT

public:
  ModelEvaluationItem(const Analysis & analysis);

  virtual void update(Observable * source, const OT::String & message);
};
}
#endif