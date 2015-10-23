// DistributionAnalysisWizard.hxx

#ifndef DISTRIBUTIONANALYSISWIZARD_H
#define DISTRIBUTIONANALYSISWIZARD_H

#include "OTStudy.hxx"

#include <QWizard>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QSpinBox>

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
  void nbSimuChanged(int);
  void setCalculus(Calculus & calculus);

private:
  Calculus calculus_;
  OTStudy * OTStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QWidget * advancedWidgets_;
  QWidget * monteCarloWidget_;
  QWidget * taylorWidget_;
  QSpinBox * nbSimuSpin_;
  QSpinBox * seedSpin_;
};
}
#endif