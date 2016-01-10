// ModelEvaluationResultWindow.hxx

#ifndef MODELEVALUATIONRESULTWINDOW_H
#define MODELEVALUATIONRESULTWINDOW_H

#include "OTguiSubWindow.hxx"
#include "otgui/ModelEvaluationItem.hxx"
#include "otgui/ModelEvaluationResult.hxx"

namespace OTGUI {
class ModelEvaluationResultWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  ModelEvaluationResultWindow(ModelEvaluationItem * item);

protected:
  void buildInterface();

private:
  ModelEvaluationResult result_;
};
}
#endif