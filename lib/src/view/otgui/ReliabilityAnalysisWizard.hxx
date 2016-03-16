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

  QString getAnalysisName() const;
  void validate();

protected:
  void buildInterface();

public slots:
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
  QWidget * monteCarloWidget_;
  QSpinBox * maxiOuterSamplingSpinbox_;
  QDoubleSpinBox * maxiCoefficientOfVariationSpinbox_;
  QSpinBox * seedSpinbox_;
  QSpinBox * blockSizeSpinbox_;
};
}
#endif