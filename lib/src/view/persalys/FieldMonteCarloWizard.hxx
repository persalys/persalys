//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_FIELDMONTECARLOWIZARD_HXX
#define PERSALYS_FIELDMONTECARLOWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/StopCriteriaGroupBox.hxx"
#include "persalys/BlockSizeGroupBox.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API FieldMonteCarloWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestFieldMonteCarloWizard;

public:
  FieldMonteCarloWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual bool validateCurrentPage();

protected:
  void buildInterface();

private:
  OutputsSelectionGroupBox * outputsGroupBox_;
  OT::Description interestVariables_;
  StopCriteriaGroupBox * stopCriteriaGroupBox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  DoubleSpinBox * klSpinbox_;
  QSpinBox * seedSpinbox_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
