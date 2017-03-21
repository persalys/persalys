//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
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
#ifndef OTGUI_METAMODELANALYSISWIZARD_HXX
#define OTGUI_METAMODELANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/KrigingAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"
#include "otgui/DoubleSpinBox.hxx"

#include <QLabel>
#include <QLineEdit>

namespace OTGUI {
class OTGUI_API MetaModelAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum Method {chaos, kriging};

  MetaModelAnalysisWizard(const OTStudy& otStudy, const DesignOfExperiment& designOfExperiment, QWidget* parent=0);
  MetaModelAnalysisWizard(const Analysis& analysis, QWidget* parent=0);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();
  void updateScaleLineEdit();

public slots:
  void updateMethodWidgets(int);
  void chaosDegreeChanged(int);
  void sparseChaosChanged(bool);
  void looValidationChanged(bool);
  void setInterestVariables(QStringList);
  void openScaleDefinitionWizard();
  void updateCovarianceModel(int);
  void updateOptimizeParameters(bool);
  void updateAmplitude(double);
  void updateMaternParameterNu(double);
  void updateGeneralizedModelParameterP(double);
  void updateBasis(int);

private:
  FunctionalChaosAnalysis chaos_;
  KrigingAnalysis kriging_;
  OutputsSelectionGroupBox * outputsGroupBox_;
  QGroupBox * chaosParametersBox_;
  CollapsibleGroupBox * chaosAdvancedParamGroupBox_;
  QGroupBox * krigingParametersBox_;
  CollapsibleGroupBox * krigingAdvancedParamGroupBox_;
  QLineEdit * scaleLineEdit_;
  DoubleSpinBox * amplitudeSpinBox_;
  QLabel * maternParameterNuLabel_;
  DoubleSpinBox * maternParameterNuSpinBox_;
  QLabel * generalizedModelParameterPLabel_;
  DoubleSpinBox * generalizedModelParameterPSpinBox_;
  QLabel * errorMessageLabel_;
};
}
#endif