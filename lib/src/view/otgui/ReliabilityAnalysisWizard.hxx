//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a reliability analysis
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_RELIABILITYANALYSISWIZARD_HXX
#define OTGUI_RELIABILITYANALYSISWIZARD_HXX

#include "otgui/LimitState.hxx"
#include "otgui/AnalysisWizard.hxx"
#include "otgui/StopCriteriaGroupBox.hxx"
#include "otgui/BlockSizeGroupBox.hxx"

#include <QButtonGroup>
#include <QLabel>

namespace OTGUI {
class OTGUI_API ReliabilityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo/*, FORM, SORM*/};

  ReliabilityAnalysisWizard(const OTStudy& otStudy, const LimitState & limitState);
  ReliabilityAnalysisWizard(const Analysis & analysis);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void maxiCoefficientOfVariationChanged(double);
  void maxiTimeChanged(int);
  void maxiCallsChanged(int);
  void blockSizeChanged(double);
  void seedChanged(int);

private:
  QButtonGroup * methodGroup_;
  QWidget * monteCarloWidget_;
  StopCriteriaGroupBox * stopCriteriaGroupBox_;
  QSpinBox * seedSpinbox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  QLabel * errorMessageLabel_;
};
}
#endif