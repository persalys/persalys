//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_INFERENCEWIZARD_HXX
#define OTGUI_INFERENCEWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/InferenceAnalysis.hxx"
#include "otgui/VariablesSelectionTableModel.hxx"
#include "otgui/TemporaryLabel.hxx"
#include "otgui/DoubleSpinBox.hxx"

#include <openturns/FittingTest.hxx>

namespace OTGUI
{
class OTGUI_API InferenceWizard : public AnalysisWizard
{
  Q_OBJECT

public :
  InferenceWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual bool validateCurrentPage();

protected:
  void initialize();
  void buildInterface();

public slots:
  void selectedVarChanged(QModelIndex, QModelIndex);
  void updateDistListForVar(QStringList);
  void updateInterestVar(OT::Description, OT::String);
signals:
  void currentVarChanged(int);
  void currentVarChecked(bool);

private:
  OT::String currentVarName_;
  InferenceAnalysis inference_;
  OT::Description interestVar_;
  std::map<OT::String, OT::FittingTest::DistributionFactoryCollection> distFactoriesForEachInterestVar_;
  DoubleSpinBox * levelSpinbox_;
  TemporaryLabel * errorMessageLabel_;
  bool pageValidity_;
  VariablesSelectionTableModel * varTableModel_;
};
}
#endif
