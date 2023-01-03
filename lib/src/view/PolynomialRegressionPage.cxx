//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a linear model analysis
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/MetaModelAnalysisWizard.hxx"

#include "persalys/ParametersDefinitionWizard.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/PolynomialRegressionAnalysis.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/OTBase.hxx>

#include <QToolButton>
#include <QVBoxLayout>

using namespace OT;

namespace PERSALYS
{

PolynomialRegressionPage::PolynomialRegressionPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void PolynomialRegressionPage::buildInterface()
{
  setTitle(tr("Linear regression parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // Kriging parameters group
  QGroupBox * parametersBox = new QGroupBox(tr("Parameters"));
  QGridLayout * parametersLayout = new QGridLayout(parametersBox);

  // correlation model
  parametersLayout->addWidget(new QLabel(tr("Degree")), 0, 0);
  degreeSpinBox_ = new QSpinBox;
  degreeSpinBox_->setMinimum(1);
  degreeSpinBox_->setMaximum(2);
  degreeSpinBox_->setValue(1);
  parametersLayout->addWidget(degreeSpinBox_, 0, 1);

  // Interaction
  parametersLayout->addWidget(new QLabel(tr("Interaction")), 1, 0);
  interactionCheckBox_ = new QCheckBox;
  parametersLayout->addWidget(interactionCheckBox_, 1, 1);

  pageLayout->addWidget(parametersBox);

  initialize(PolynomialRegressionAnalysis());
}


void PolynomialRegressionPage::initialize(const Analysis& analysis)
{
  const PolynomialRegressionAnalysis * analysis_ptr = dynamic_cast<const PolynomialRegressionAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  const Description inputNames = analysis_ptr->getDesignOfExperiment().getInputSample().getMarginal(analysis_ptr->getDesignOfExperiment().getEffectiveInputIndices()).getDescription();
  inputsNames_ = inputNames;

  degreeSpinBox_->setValue(analysis_ptr->getDegree());
  interactionCheckBox_->setChecked(analysis_ptr->getInteraction());
}


Analysis PolynomialRegressionPage::getAnalysis(const String& name, const DesignOfExperiment& doe) const
{
  PolynomialRegressionAnalysis analysis(name, doe);
  analysis.setDegree(degreeSpinBox_->value());
  analysis.setInteraction(interactionCheckBox_->isChecked());
  return analysis;
}

}
