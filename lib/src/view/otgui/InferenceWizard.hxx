//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_INFERENCEWIZARD_HXX
#define OTGUI_INFERENCEWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/InferenceAnalysis.hxx"
#include "otgui/DistributionsForInferenceWidget.hxx"

#include <openturns/FittingTest.hxx>

#include <QModelIndex>
#include <QLabel>

namespace OTGUI {
class OTGUI_API InferenceWizard : public AnalysisWizard
{
  Q_OBJECT

public :
  InferenceWizard(const OTStudy& otStudy, const DesignOfExperiment & designOfExperiment, QWidget* parent=0);
  InferenceWizard(const Analysis& analysis, QWidget* parent=0);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void selectedVariableChanged(QModelIndex, QModelIndex);
  void updateDistributionsListForVariable(QStringList);
  void updateInterestVariables(OT::Description, OT::String);
  void levelChanged(double);
signals:
  void currentVariableChanged(int);
  void currentVariableChecked(bool);

private:
  OT::String currentVariableName_;
  InferenceAnalysis inference_;
  std::map<OT::String, OT::FittingTest::DistributionFactoryCollection> distFactoriesForEachInterestVar_;
  QLabel * errorMessageLabel_;
  bool pageValidity_;
};
}
#endif