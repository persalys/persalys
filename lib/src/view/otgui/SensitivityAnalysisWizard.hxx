// SensitivityAnalysisWizard.hxx

#ifndef SENSITIVITYANALYSISWIZARD_H
#define SENSITIVITYANALYSISWIZARD_H

#include "otgui/OTStudy.hxx"

#include <QWizard>
#include <QButtonGroup>
#include <QSpinBox>

namespace OTGUI {
class SensitivityAnalysisWizard : public QWizard
{
  Q_OBJECT

public:
  enum Method {Sobol, SRC};

  SensitivityAnalysisWizard(OTStudy * study, const PhysicalModel & physicalModel);
  SensitivityAnalysisWizard(const Calculus & calculus);

  void validate();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets();
  void nbSimuChanged(int);
signals:
  void calculusChanged(const Calculus & calculus);

private:
  Calculus calculus_;
  OTStudy * OTStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QSpinBox * nbSimuSpinbox_;
};
}
#endif