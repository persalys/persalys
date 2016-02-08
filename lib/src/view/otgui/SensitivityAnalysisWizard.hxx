// SensitivityAnalysisWizard.hxx

#ifndef OTGUI_SENSITIVITYANALYSISWIZARD_H
#define OTGUI_SENSITIVITYANALYSISWIZARD_H

#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>
#include <QSpinBox>
#include <QGroupBox>

namespace OTGUI {
class SensitivityAnalysisWizard : public OTguiWizard
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
  void showHideAdvancedWidgets(bool);
  void nbSimuChanged(int);
  void seedChanged(int);
signals:
  void analysisChanged(const Analysis & analysis);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QGroupBox * advancedGroup_;
  QWidget * advancedWidgets_;
  QSpinBox * nbSimuSpinbox_;
  QSpinBox * seedSpinbox_;
};
}
#endif