// CentralTendencyWizard.hxx

#ifndef DISTRIBUTIONANALYSISWIZARD_H
#define DISTRIBUTIONANALYSISWIZARD_H

#include "otgui/OTStudy.hxx"

#include <QWizard>
#include <QButtonGroup>
#include <QSpinBox>
#include <QCheckBox>

namespace OTGUI {
class CentralTendencyWizard : public QWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, QuadraticCumul};

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
signals:
  void analysisChanged(const Analysis & analysis);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QWidget * advancedWidgets_;
  QWidget * monteCarloWidget_;
  QWidget * quadraticCumulWidget_;
  QSpinBox * nbSimuSpinbox_;
  QSpinBox * seedSpinbox_;
  QCheckBox * confidenceIntervalCheckBox_;
  QDoubleSpinBox * levelConfidenceIntervalSpinbox_;
};
}
#endif