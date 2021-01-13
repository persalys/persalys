//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiments
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PERSALYS_PROBABILISTICDESIGNPAGE_HXX
#define PERSALYS_PROBABILISTICDESIGNPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/LogSpinBox.hxx"

#include <QWizardPage>
#include <QButtonGroup>
#include <QComboBox>

namespace PERSALYS
{
class PERSALYS_VIEW_API ProbabilisticDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  enum design {LHS, SALHS, MCLHS, MonteCarlo, QuasiMonteCarlo};

  ProbabilisticDesignPage(QWidget* parent = 0);

  void initialize(const Analysis& designOfExperiment);
  Analysis getAnalysis(const OT::String& name, const PhysicalModel& model) const;

protected:
  void buildInterface();

public slots:
signals:
  void designOfExperimentChanged(const Analysis & designOfExperiment);
  void showTime();

private:
  QButtonGroup * designsGroup_;
  QComboBox * optimComboBox_;
  QComboBox * spaceFillingComboBox_;
  LogSpinBox * sampleSizeSpinbox_;
  LogSpinBox * mcLhsSizeSB_;
  QSpinBox * seedSpinbox_;
  OT::Scalar modelEvalTime_;
};
}
#endif
