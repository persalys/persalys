// ModelEvaluationWizard.hxx

#ifndef MODELEVALUATIONWIZARD_H
#define MODELEVALUATIONWIZARD_H

#include "otgui/InputTableModel.hxx"
#include "otgui/OTStudy.hxx"

#include <QWizard>
#include <QTableWidget>

namespace OTGUI {
class ModelEvaluationWizard : public QWizard
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