// CentralTendencyWizard.hxx

#ifndef OTGUI_CENTRALTENDENCYWIZARD_H
#define OTGUI_CENTRALTENDENCYWIZARD_H

#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>
#include <QSpinBox>
#include <QCheckBox>

namespace OTGUI {
class CentralTendencyWizard : public OTguiWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, TaylorExpansionsMoments};

  CentralTendencyWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  CentralTendencyWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets();
  void showHideAdvancedWidgets(bool);
  void confidenceIntervalRequired(bool);
  void nbSimuChanged(int);
  void levelConfidenceIntervalChanged(double);
  void seedChanged(int);
signals:
  void analysisChanged(const Analysis & analysis);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QWidget * advancedWidgets_;
  QWidget * monteCarloWidget_;
  QWidget * TaylorExpansionsWidget_;
  QSpinBox * nbSimuSpinbox_;
  QSpinBox * seedSpinbox_;
  QCheckBox * confidenceIntervalCheckBox_;
  QDoubleSpinBox * levelConfidenceIntervalSpinbox_;
};
}
#endif