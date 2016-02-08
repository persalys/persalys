// ModelEvaluationWizard.hxx

#ifndef OTGUI_MODELEVALUATIONWIZARD_H
#define OTGUI_MODELEVALUATIONWIZARD_H

#include "otgui/InputTableModel.hxx"
#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QWizard>
#include <QTableWidget>

namespace OTGUI {
class ModelEvaluationWizard : public OTguiWizard
{
  Q_OBJECT

public:
  ModelEvaluationWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  ModelEvaluationWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void inputValueChanged(double value);

private:
  QTableWidget * table_;
  Analysis analysis_;
  OTStudy * otStudy_;
};
}
#endif