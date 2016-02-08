// ReliabilityAnalysisWizard.hxx

#ifndef OTGUI_RELIABILITYANALYSISWIZARD_H
#define OTGUI_RELIABILITYANALYSISWIZARD_H

#include "otgui/OTStudy.hxx"
#include "otgui/LimitState.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>
#include <QSpinBox>

namespace OTGUI {
class ReliabilityAnalysisWizard : public OTguiWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo/*, FORM, SORM*/};

  ReliabilityAnalysisWizard(OTStudy * otStudy, const LimitState & limitState);
  ReliabilityAnalysisWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void showHideAdvancedWidgets(bool);
  void maxiOuterSamplingChanged(int);
  void maxiCoefficientOfVariationChanged(double);
  void blockSizeChanged(int);
  void seedChanged(int);
signals:
  void analysisChanged(const Analysis & analysis);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
//   TODO: LimitState limitState_; later when implementing FORM/SORM

  QButtonGroup * methodGroup_;
  QWidget * advancedWidgets_;
  QWidget * monteCarloWidget_;
  QSpinBox * maxiOuterSamplingSpinbox_;
  QDoubleSpinBox * maxiCoefficientOfVariationSpinbox_;
  QSpinBox * seedSpinbox_;
  QSpinBox * blockSizeSpinbox_;
};
}
#endif