//                                               -*- C++ -*-
/**
 *  @brief QWizard to select a design of experiments
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTEVALUATIONWIZARD_HXX
#define PERSALYS_DESIGNOFEXPERIMENTEVALUATIONWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/BlockSizeGroupBox.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QStandardItemModel>
#include <QComboBox>

Q_DECLARE_METATYPE(PERSALYS::Analysis)

namespace PERSALYS
{

class PERSALYS_VIEW_API DesignOfExperimentEvaluationWizard : public Wizard
{
  Q_OBJECT

  friend class TestDesignOfExperimentEvaluationWizard;

public:
  DesignOfExperimentEvaluationWizard(const Analysis& analysis, bool isGeneralWizard = false, QWidget* parent = 0);

  Analysis getAnalysis() const;
  virtual bool validateCurrentPage();

protected slots:
  void buildInterface();
  void updateWidgets();

private:
  QComboBox * doesComboBox_;
  QStandardItemModel * doesComboBoxModel_;
  QLabel * doeLabel_;
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
