//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a kriging analysis
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_KRIGINGPAGE_HXX
#define OTGUI_KRIGINGPAGE_HXX

#include "otgui/Analysis.hxx"
#include "otgui/DesignOfExperiment.hxx"
#include "otgui/DoubleSpinBox.hxx"

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

namespace OTGUI
{

class OTGUI_API KrigingPage : public QWizardPage
{
  Q_OBJECT

public:
  KrigingPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const DesignOfExperiment& doe) const;

protected:
  void buildInterface();
  void updateScaleLineEdit();

public slots:
  void updateCovarianceModel(int);
  void updateCovarianceModel(DesignOfExperiment);
  void openScaleDefinitionWizard();

private:
  QComboBox * covarianceModelComboBox_;
  QLabel * maternParameterNuLabel_;
  DoubleSpinBox * maternParameterNuSpinBox_;
  QLabel * generalizedModelParameterPLabel_;
  DoubleSpinBox * generalizedModelParameterPSpinBox_;
  QComboBox * basisTypeComboBox_;
  QCheckBox * optimizationCheckBox_;
  QLineEdit * scaleLineEdit_;
  OT::Description inputsNames_;
  OT::Point scales_;
  DoubleSpinBox * amplitudeSpinBox_;
};
}
#endif
