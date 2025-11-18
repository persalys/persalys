//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a linear model analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

QString PolynomialRegressionPage::FORWARD   = tr("forward");
QString PolynomialRegressionPage::BACKWARD  = tr("backward");
QString PolynomialRegressionPage::BOTH      = tr("both");
QString PolynomialRegressionPage::AIC       = tr("AIC");
QString PolynomialRegressionPage::BIC       = tr("BIC");

PolynomialRegressionPage::PolynomialRegressionPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void PolynomialRegressionPage::buildInterface()
{
  setTitle(tr("Linear regression parameters"));

  auto * pageLayout = new QVBoxLayout(this);

  auto * parametersBox = new QGroupBox(tr("Parameters"));
  auto * parametersLayout = new QGridLayout(parametersBox);

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

  // Stepwise
  parametersLayout->addWidget(new QLabel(tr("Stepwise")), 2, 0);
  stepwiseCheckBox_ = new QCheckBox;
  parametersLayout->addWidget(stepwiseCheckBox_, 2, 1);

  // Advenced parameters
  auto * advancedGroupBox = new CollapsibleGroupBox(tr("Advanced parameters"));
  auto * advancedGroupBoxLayout = new QGridLayout(advancedGroupBox);
  parametersLayout->addWidget(advancedGroupBox, 3, 0);

  advancedGroupBoxLayout->addWidget(new QLabel(tr("Direction")), 0, 0);
  directionComboBox_ = new QComboBox();
  directionComboBox_->addItem(FORWARD, LinearModelStepwiseAlgorithm::FORWARD);
  directionComboBox_->addItem(BACKWARD, LinearModelStepwiseAlgorithm::BACKWARD);
  directionComboBox_->addItem(BOTH, LinearModelStepwiseAlgorithm::BOTH);
  advancedGroupBoxLayout->addWidget(directionComboBox_, 0, 1);

  advancedGroupBoxLayout->addWidget(new QLabel(tr("Penalty criteria")), 1, 0);
  penaltyCriteriaComboBox_ = new QComboBox();
  penaltyCriteriaComboBox_->addItem(BIC, PolynomialRegressionAnalysis::BIC);
  penaltyCriteriaComboBox_->addItem(AIC, PolynomialRegressionAnalysis::AIC);
  advancedGroupBoxLayout->addWidget(penaltyCriteriaComboBox_, 1, 1);

#if QT_VERSION < 0x060700
  connect(stepwiseCheckBox_, &QCheckBox::stateChanged, [advancedGroupBox] (int state) {
#else
  connect(stepwiseCheckBox_, &QCheckBox::checkStateChanged, [advancedGroupBox] (Qt::CheckState state) {
#endif
    advancedGroupBox->setVisible(state);
  });

  pageLayout->addWidget(parametersBox);
}


void PolynomialRegressionPage::initialize(const Analysis& analysis)
{
  const auto * analysis_ptr = dynamic_cast<const PolynomialRegressionAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  degreeSpinBox_->setValue((int) analysis_ptr->getDegree());
  interactionCheckBox_->setChecked(analysis_ptr->getInteraction());
  stepwiseCheckBox_->setChecked(analysis_ptr->getStepwise());
  switch (analysis_ptr->getDirection())
  {
    case LinearModelStepwiseAlgorithm::FORWARD:
      directionComboBox_->setCurrentText(FORWARD);
      break;
    
    case LinearModelStepwiseAlgorithm::BACKWARD:
    directionComboBox_->setCurrentText(BACKWARD);
    break;
  
    case LinearModelStepwiseAlgorithm::BOTH:
    directionComboBox_->setCurrentText(BOTH);
    break;
  }

  analysis_ptr->getPenalty() == PolynomialRegressionAnalysis::PenaltyCriteria::AIC  ? penaltyCriteriaComboBox_->setCurrentText(AIC)
                                                                                    : penaltyCriteriaComboBox_->setCurrentText(BIC);
}


Analysis PolynomialRegressionPage::getAnalysis(const String& name, const DesignOfExperiment& doe) const
{
  PolynomialRegressionAnalysis analysis(name, doe);
  analysis.setDegree(degreeSpinBox_->value());
  analysis.setInteraction(interactionCheckBox_->isChecked());
  analysis.setStepwise(stepwiseCheckBox_->isChecked());
  analysis.setDirection(static_cast<LinearModelStepwiseAlgorithm::Direction>(directionComboBox_->currentData().toInt()));
  analysis.setPenalty(static_cast<PolynomialRegressionAnalysis::PenaltyCriteria>(penaltyCriteriaComboBox_->currentData().toInt()));

  return analysis;
}

}
