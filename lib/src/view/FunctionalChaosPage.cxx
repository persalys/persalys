//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/FunctionalChaosPage.hxx"

#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/CollapsibleGroupBox.hxx"

#include <openturns/SpecFunc.hxx>

#include <QVBoxLayout>
#include <QGroupBox>

using namespace OT;

namespace PERSALYS
{

FunctionalChaosPage::FunctionalChaosPage(QWidget* parent)
  : QWizardPage(parent)
  , inputSampleSize_(0)
  , inputSampleDimension_(0)
  , chaosDegreeSpinbox_(0)
  , sparseCheckBox_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void FunctionalChaosPage::buildInterface()
{
  setTitle(tr("Functional chaos parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // Chaos parameters group
  QGroupBox * parametersBox = new QGroupBox(tr("Parameters"));
  QGridLayout * chaosParametersLayout = new QGridLayout(parametersBox);

  // -- Polynomial degree
  chaosParametersLayout->addWidget(new QLabel(tr("Polynomial degree")), 0, 0);
  chaosDegreeSpinbox_ = new QSpinBox;
  chaosDegreeSpinbox_->setRange(1, 30);
  chaosParametersLayout->addWidget(chaosDegreeSpinbox_, 0, 1);

  basisLabel_ = new QLabel;
  chaosParametersLayout->addWidget(basisLabel_, 1, 0);
  basisSizeLabel_ = new QLabel;
  chaosParametersLayout->addWidget(basisSizeLabel_, 1, 1);

  pageLayout->addWidget(parametersBox);

  // functional chaos advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedParamGroupBoxLayout = new QGridLayout(advancedParamGroupBox);

  sparseCheckBox_ = new QCheckBox(tr("Sparse"));
  advancedParamGroupBoxLayout->addWidget(sparseCheckBox_, 0, 0);

  advancedParamGroupBox->setExpanded(false);
  pageLayout->addWidget(advancedParamGroupBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  initialize(FunctionalChaosAnalysis());
}


void FunctionalChaosPage::initialize(const Analysis& analysis)
{
  const FunctionalChaosAnalysis * analysis_ptr = dynamic_cast<const FunctionalChaosAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  inputSampleSize_ = analysis_ptr->getDesignOfExperiment().getSample().getSize();
  const UnsignedInteger inputDimension = analysis_ptr->getDesignOfExperiment().getEffectiveInputIndices().getSize();
  inputSampleDimension_ = inputDimension;

  chaosDegreeSpinbox_->setValue(analysis_ptr->getChaosDegree());
  sparseCheckBox_->setChecked(analysis_ptr->getSparseChaos());
  updateBasisLabel();
  updateBasisSizeLabel();

  connect(chaosDegreeSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(updateBasisSizeLabel()));
  connect(sparseCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(updateBasisLabel()));
}

void FunctionalChaosPage::updateBasisLabel()
{
  const QString label = sparseCheckBox_->isChecked() ? tr("Full basis size") : tr("Basis size");
  basisLabel_->setText(label);
}

void FunctionalChaosPage::updateBasisSizeLabel()
{
  const int basisSize = SpecFunc::BinomialCoefficient(inputSampleDimension_+chaosDegreeSpinbox_->value(), chaosDegreeSpinbox_->value());
  basisSizeLabel_->setText(QString::number(basisSize));
}
Analysis FunctionalChaosPage::getAnalysis(const String& name, const DesignOfExperiment& doe) const
{
  FunctionalChaosAnalysis analysis(name, doe);

  analysis.setChaosDegree(chaosDegreeSpinbox_->value());
  analysis.setSparseChaos(sparseCheckBox_->isChecked());

  return analysis;
}


void FunctionalChaosPage::updateInputSampleSize(DesignOfExperiment doe)
{
  errorMessageLabel_->reset();
  inputSampleSize_ = doe.getInputSample().getSize();
}


bool FunctionalChaosPage::validatePage()
{
  if (!sparseCheckBox_->isChecked())
  {
    const UnsignedInteger degree = chaosDegreeSpinbox_->value();
    const UnsignedInteger n = degree + inputSampleDimension_;
    // compute binomial coefficient
    const UnsignedInteger minimumSize = SpecFunc::BinomialCoefficient(n, degree);
    if (inputSampleSize_ < minimumSize)
    {
      const QString errorMessage = tr("Design of experiments size too small : %1. It must be greater or equal to C(degree+nbInputs, degree) = %2")
                                   .arg(inputSampleSize_)
                                   .arg(minimumSize);
      errorMessageLabel_->setErrorMessage(errorMessage);
      return false;
    }
  }
  return QWizardPage::validatePage();
}
}
