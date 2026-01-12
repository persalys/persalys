//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiments
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/ErrorWidget.hxx"

#include <QWizardPage>
#include <QLabel>
#include <QButtonGroup>
#include <QComboBox>

namespace PERSALYS
{
class PERSALYS_VIEW_API ProbabilisticDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  enum DesignType {MonteCarlo, LHS, SALHS, MCLHS, QuasiMonteCarlo};

  explicit ProbabilisticDesignPage(QWidget* parent = nullptr);

  void initialize(const Analysis& designOfExperiment);
  Analysis getAnalysis(const OT::String& name, const PhysicalModel& model) const;

private:
  void buildInterface();
  void updateEstimatedDuration();

private:
  OT::Scalar      modelEvalTime_        = 0.0;
  int             blockSize_            = 1;
  ErrorWidget   * lhsWarningLabel_      = nullptr;
  QButtonGroup  * designButtonsGroup_   = nullptr;
  QComboBox     * optimComboBox_        = nullptr;
  LogSpinBox    * mcLhsSizeSB_          = nullptr;
  QComboBox     * spaceFillingComboBox_ = nullptr;
  LogSpinBox    * sampleSizeSpinBox_    = nullptr;
  QSpinBox      * seedSpinBox_          = nullptr;
  QLabel        * sampleTimeLabel_      = nullptr;
  QLabel        * sampleTimeValueLabel_ = nullptr;
};

} // namespace PERSALYS
#endif
