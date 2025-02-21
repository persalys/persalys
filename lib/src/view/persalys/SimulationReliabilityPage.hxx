//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define simulation reliability analysis
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
#ifndef PERSALYS_SIMULATIONRELIABILITYPAGE_HXX
#define PERSALYS_SIMULATIONRELIABILITYPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/LimitState.hxx"
#include "persalys/StopCriteriaGroupBox.hxx"
#include "persalys/BlockSizeGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>

namespace PERSALYS
{
class PERSALYS_VIEW_API SimulationReliabilityPage : public QWizardPage
{
  Q_OBJECT

public:
  SimulationReliabilityPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  void updateAnalysis(const Analysis& analysis);

  virtual bool validatePage();

protected:
  void buildInterface();

private:
  StopCriteriaGroupBox * stopCriteriaGroupBox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  QSpinBox * seedSpinbox_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
