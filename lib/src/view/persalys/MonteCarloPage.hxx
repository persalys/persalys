//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define monte carlo analysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_MONTECARLOPAGE_HXX
#define PERSALYS_MONTECARLOPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/StopCriteriaGroupBox.hxx"
#include "persalys/BlockSizeGroupBox.hxx"
#include "persalys/ErrorWidget.hxx"

#include <QWizardPage>

namespace PERSALYS
{
class PERSALYS_VIEW_API MonteCarloPage : public QWizardPage
{
  Q_OBJECT

public:
  MonteCarloPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const PhysicalModel& physicalModel) const;

  int nextId() const override;
  bool validatePage() override;

protected:
  void buildInterface();

protected slots:
  void updateLevelCISpinbox();

private:
  StopCriteriaGroupBox  * stopCriteriaGroupBox_           = nullptr;
  BlockSizeGroupBox     * blockSizeGroupBox_              = nullptr;
  QCheckBox             * confidenceIntervalCheckBox_     = nullptr;
  DoubleSpinBox         * levelConfidenceIntervalSpinbox_ = nullptr;
  QSpinBox              * seedSpinbox_                    = nullptr;
  QCheckBox             * allowFailedEvaluationsCheckBox_ = nullptr;
  ErrorWidget           * errorWidget_              = nullptr;
};
}
#endif
