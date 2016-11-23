//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
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
#ifndef OTGUI_SENSITIVITYANALYSISWIZARD_HXX
#define OTGUI_SENSITIVITYANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/SobolAnalysis.hxx"
#include "otgui/SRCAnalysis.hxx"
#include "otgui/StopCriteriaGroupBox.hxx"
#include "otgui/BlockSizeGroupBox.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"

#include <QLabel>

namespace OTGUI {
class OTGUI_API SensitivityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum Method {Sobol, SRC};

  SensitivityAnalysisWizard(const OTStudy& otStudy, const PhysicalModel & physicalModel);
  SensitivityAnalysisWizard(const Analysis & analysis);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets(int);
  void maxiCoefficientOfVariationChanged(double);
  void maxiTimeChanged(int);
  void maxiCallsChanged(int);
  void sampleSizeChanged(int);
  void seedChanged(int);
  void blockSizeChanged(double);
  void setInterestVariables(QStringList);

private:
  SobolAnalysis sobolAnalysis_;
  SRCAnalysis srcAnalysis_;
  OutputsSelectionGroupBox * outputsGroupBox_;
  QWidget * sobolWidgets_;
  StopCriteriaGroupBox * stopCriteriaGroupBox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  QWidget * srcWidgets_;
  QLabel * totalNbSimuLabel_;
  QLabel * errorMessageLabel_;
};
}
#endif