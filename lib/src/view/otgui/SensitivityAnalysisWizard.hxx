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

  SensitivityAnalysisWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  SensitivityAnalysisWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets();
  void nbSimuChanged(int);
signals:
  void analysisChanged(const Analysis & analysis);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QSpinBox * nbSimuSpinbox_;
};
}
#endif