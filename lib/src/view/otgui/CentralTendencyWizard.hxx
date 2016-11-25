//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
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
#ifndef OTGUI_CENTRALTENDENCYWIZARD_HXX
#define OTGUI_CENTRALTENDENCYWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/StopCriteriaGroupBox.hxx"
#include "otgui/BlockSizeGroupBox.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"
#include "otgui/MonteCarloAnalysis.hxx"

#include <QLabel>

namespace OTGUI {
class OTGUI_API CentralTendencyWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, TaylorExpansionMoments};

  CentralTendencyWizard(const OTStudy& otStudy, const PhysicalModel & physicalModel, QWidget* parent=0);
  CentralTendencyWizard(const Analysis & analysis, QWidget* parent=0);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets(int);
  void maxiCoefficientOfVariationChanged(double);
  void maxiTimeChanged(int);
  void maxiCallsChanged(int);
  void confidenceIntervalRequired(bool);
  void levelConfidenceIntervalChanged(double);
  void seedChanged(int);
  void blockSizeChanged(double);
  void setInterestVariables(QStringList);

private:
  MonteCarloAnalysis MCAnalysis_;
  TaylorExpansionMomentsAnalysis taylorAnalysis_;
  OutputsSelectionGroupBox * outputsGroupBox_;
  QWidget * monteCarloWidget_;
  StopCriteriaGroupBox * stopCriteriaGroupBox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  QWidget * TaylorExpansionsWidget_;
  QLabel * errorMessageLabel_;
};
}
#endif