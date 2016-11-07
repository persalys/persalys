//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
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
#ifndef OTGUI_METAMODELANALYSISWIZARD_HXX
#define OTGUI_METAMODELANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"

#include <QLabel>
#include <QGroupBox>

namespace OTGUI {
class OTGUI_API MetaModelAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum Method {chaos, kriging};

  MetaModelAnalysisWizard(const OTStudy& otStudy, const DesignOfExperiment & designOfExperiment);
  MetaModelAnalysisWizard(const Analysis & analysis);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets(int);
  void chaosDegreeChanged(int);
  void sparseChaosChanged(bool);
  void looValidationChanged(bool);
  void setOutputsToAnalyse(QStringList);

private:
  FunctionalChaosAnalysis chaos_;
//   Kriging kriging_;
  OutputsSelectionGroupBox * outputsGroupBox_;
  QGroupBox * chaosParametersBox_;
  CollapsibleGroupBox * chaosAdvancedParamGroupBox_;
//   QWidget * krigingWidgets_;
  QLabel * errorMessageLabel_;
};
}
#endif