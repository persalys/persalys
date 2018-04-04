//                                               -*- C++ -*-
/**
 *  @brief QWizardPage for meta model validation
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
#include "otgui/MetaModelValidationPage.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"

#include <QGridLayout>
#include <QGroupBox>

using namespace OT;

namespace OTGUI
{

MetaModelValidationPage::MetaModelValidationPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void MetaModelValidationPage::buildInterface()
{
  setTitle(tr("Metamodel validation"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * validationGroupBox = new QGroupBox(tr("Computation of the predictivity factor Q2"));
  QGridLayout * validationLayout = new QGridLayout(validationGroupBox);
  int row = -1;

  // Analytical
  analyticalCheckBox_ = new QCheckBox;
  validationLayout->addWidget(analyticalCheckBox_, ++row, 0);
  validationLayout->addWidget(new QLabel(tr("Analytically")), row, 1);

  // Test Sample
  testSampleCheckBox_ = new QCheckBox;
  validationLayout->addWidget(testSampleCheckBox_, ++row, 0);
  validationLayout->addWidget(new QLabel(tr("By splitting data into training/test samples")), row, 1);

  QHBoxLayout * testSampleLayout = new QHBoxLayout;
  testSampleLayout->addWidget(new QLabel(tr("Test sample size")));
  percentageOfPointsSpinBox_ = new UIntSpinBox;
  percentageOfPointsSpinBox_->setSuffix("%");
  percentageOfPointsSpinBox_->setMinimum(10);
  percentageOfPointsSpinBox_->setMaximum(50);
  percentageOfPointsSpinBox_->setSingleStep(5);
  connect(testSampleCheckBox_, SIGNAL(clicked(bool)), percentageOfPointsSpinBox_, SLOT(setEnabled(bool)));
  connect(percentageOfPointsSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(percentageValueChanged(double)));
  testSampleLayout->addWidget(percentageOfPointsSpinBox_);
  testSampleLayout->addWidget(new QLabel(tr("of the design of experiments")));
  validationLayout->addLayout(testSampleLayout, ++row, 1);

  trainingSampleSize_ = new QLabel;
  validationLayout->addWidget(trainingSampleSize_, ++row, 1);

  testSampleLayout = new QHBoxLayout;
  testSampleLayout->addWidget(new QLabel(tr("Seed")));
  seedTestSampleSpinBox_ = new UIntSpinBox;
  connect(testSampleCheckBox_, SIGNAL(clicked(bool)), seedTestSampleSpinBox_, SLOT(setEnabled(bool)));
  testSampleLayout->addWidget(seedTestSampleSpinBox_);
  validationLayout->addLayout(testSampleLayout, ++row, 1, Qt::AlignLeft);

  // KFold
  kFoldCheckBox_ = new QCheckBox;
  validationLayout->addWidget(kFoldCheckBox_, ++row, 0);
  validationLayout->addWidget(new QLabel(tr("By K-Fold method")), row, 1);

  QHBoxLayout * kFoldLayout = new QHBoxLayout;
  kFoldLayout->addWidget(new QLabel(tr("Number of folds")));
  nbFoldsSpinBox_ = new UIntSpinBox;
  nbFoldsSpinBox_->setMinimum(2);
  connect(kFoldCheckBox_, SIGNAL(clicked(bool)), nbFoldsSpinBox_, SLOT(setEnabled(bool)));
  kFoldLayout->addWidget(nbFoldsSpinBox_);
  validationLayout->addLayout(kFoldLayout, ++row, 1, Qt::AlignLeft);

  kFoldLayout = new QHBoxLayout;
  kFoldLayout->addWidget(new QLabel(tr("Seed")));
  seedKFoldSpinBox_ = new UIntSpinBox;
  connect(kFoldCheckBox_, SIGNAL(clicked(bool)), seedKFoldSpinBox_, SLOT(setEnabled(bool)));
  kFoldLayout->addWidget(seedKFoldSpinBox_);
  validationLayout->addLayout(kFoldLayout, ++row, 1, Qt::AlignLeft);

  // LOO
  looCheckBox_ = new QCheckBox;
  validationLayout->addWidget(looCheckBox_, ++row, 0);
  validationLayout->addWidget(new QLabel(tr("By Leave-one-out method")), row, 1);

  validationLayout->setColumnStretch(2, 1);

  pageLayout->addWidget(validationGroupBox);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  initialize(FunctionalChaosAnalysis());
}


void MetaModelValidationPage::updateInputSampleSize(DesignOfExperiment doe)
{
  inputSampleSize_ = doe.getInputSample().getSize();
  percentageValueChanged(percentageOfPointsSpinBox_->value());
}


void MetaModelValidationPage::percentageValueChanged(double value)
{
  const UnsignedInteger size = inputSampleSize_ * (1 - value / 100);
  trainingSampleSize_->setText(tr("Training sample size :") + " " + (OSS() << size).str().c_str());
}


void MetaModelValidationPage::initialize(const Analysis& analysis)
{
  const MetaModelAnalysis * analysis_ptr = dynamic_cast<const MetaModelAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  inputSampleSize_ = analysis_ptr->getDesignOfExperiment().getSample().getSize();

  analyticalCheckBox_->setChecked(analysis_ptr->analyticalValidation());
  testSampleCheckBox_->setChecked(analysis_ptr->testSampleValidation());
  kFoldCheckBox_->setChecked(analysis_ptr->kFoldValidation());
  looCheckBox_->setChecked(analysis_ptr->leaveOneOutValidation());

  percentageOfPointsSpinBox_->setValue(analysis_ptr->getTestSampleValidationPercentageOfPoints());
  percentageOfPointsSpinBox_->setEnabled(testSampleCheckBox_->isChecked());
  seedTestSampleSpinBox_->setValue(analysis_ptr->getTestSampleValidationSeed());
  seedTestSampleSpinBox_->setEnabled(testSampleCheckBox_->isChecked());
  nbFoldsSpinBox_->setValue(analysis_ptr->getKFoldValidationNumberOfFolds());
  nbFoldsSpinBox_->setEnabled(kFoldCheckBox_->isChecked());
  seedKFoldSpinBox_->setValue(analysis_ptr->getKFoldValidationSeed());
  seedKFoldSpinBox_->setEnabled(kFoldCheckBox_->isChecked());
}


void MetaModelValidationPage::updateMetamodelValidation(Analysis& analysis)
{
  MetaModelAnalysis * analysis_ptr = dynamic_cast<MetaModelAnalysis*>(analysis.getImplementation().get());
  Q_ASSERT(analysis_ptr);

  analysis_ptr->setAnalyticalValidation(analyticalCheckBox_->isChecked());
  analysis_ptr->setTestSampleValidation(testSampleCheckBox_->isChecked());
  analysis_ptr->setKFoldValidation(kFoldCheckBox_->isChecked());
  analysis_ptr->setLeaveOneOutValidation(looCheckBox_->isChecked());
  analysis_ptr->setTestSampleValidationPercentageOfPoints(percentageOfPointsSpinBox_->value());
  analysis_ptr->setTestSampleValidationSeed(seedTestSampleSpinBox_->value());
  analysis_ptr->setKFoldValidationNumberOfFolds(nbFoldsSpinBox_->value());
  analysis_ptr->setKFoldValidationSeed(seedKFoldSpinBox_->value());
}


bool MetaModelValidationPage::validatePage()
{
  errorMessageLabel_->setText("");

  if (testSampleCheckBox_->isChecked() && inputSampleSize_ * percentageOfPointsSpinBox_->value() / 100 < 3)
  {
    const QString errorMessage = tr("Test sample validation: The test sample must contain at least three points. Here size * k / 100 = %1")
                                 .arg(percentageOfPointsSpinBox_->value());
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
    return false;
  }

  if (kFoldCheckBox_->isChecked() && inputSampleSize_ / nbFoldsSpinBox_->value() < 3)
  {
    const QString errorMessage = tr("K-Fold validation: each fold must contain at least three points. Here size / k = %1")
                                 .arg(inputSampleSize_ / nbFoldsSpinBox_->value());
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
    return false;
  }
  return QWizardPage::validatePage();
}
}
