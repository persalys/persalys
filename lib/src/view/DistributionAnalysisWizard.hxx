// DistributionAnalysisWizard.hxx

#ifndef DISTRIBUTIONANALYSISWIZARD_H
#define DISTRIBUTIONANALYSISWIZARD_H

#include "OTStudy.hxx"

#include <QWizard>
#include <QButtonGroup>
#include <QSpinBox>
#include <QCheckBox>

namespace OTGUI {
class DistributionAnalysisWizard : public QWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, Taylor};

  DistributionAnalysisWizard(OTStudy * study, const PhysicalModel & physicalModel);
  DistributionAnalysisWizard(const Calculus & calculus);

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
  void calculusChanged(const Calculus & calculus);

private:
  Calculus calculus_;
  OTStudy * OTStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QWidget * advancedWidgets_;
  QWidget * monteCarloWidget_;
  QWidget * taylorWidget_;
  QSpinBox * nbSimuSpinbox_;
  QSpinBox * seedSpinbox_;
  QCheckBox * confidenceIntervalCheckBox_;
  QDoubleSpinBox * levelConfidenceIntervalSpinbox_;
};
}
#endif