//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of optimization
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
#ifndef PERSALYS_ALGOOPTIMIZATIONPAGE_HXX
#define PERSALYS_ALGOOPTIMIZATIONPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QVBoxLayout>
#include <QButtonGroup>

namespace PERSALYS
{

class PERSALYS_VIEW_API OptimizationAlgoPage : public QWizardPage
{
  Q_OBJECT

  friend class TestOptimizationWizard;

public:
  OptimizationAlgoPage(QWidget* parent = 0);

  OT::Description getInterestVariables() const;

  OT::String getSolverName() const;

  virtual bool validatePage();

protected slots:
  void initialize(OptimizationAnalysis&);

private:
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  QButtonGroup * methodGroup_;
  TemporaryLabel * errorMessageLabel_;
  QVBoxLayout * pageLayout_;
};
}
#endif
