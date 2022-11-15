//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the design of experiments and the method of metamodel analysis
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
#include "persalys/MetaModelIntroPage.hxx"

#include "persalys/MetaModelAnalysisWizard.hxx"
#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{

MetaModelIntroPage::MetaModelIntroPage(QWidget* parent)
  : QWizardPage(parent)
{
  setTitle(tr("Methods to create metamodels"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // choose the limit state
  QGroupBox * doeBox = new QGroupBox(tr("Design of experiments"));
  QVBoxLayout * doeLayout = new QVBoxLayout(doeBox);

  doesComboBox_ = new QComboBox;
  doeLayout->addWidget(doesComboBox_);
  connect(doesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDesignOfExperiment(int)));
  doesComboBoxModel_ = new QStandardItemModel(doesComboBox_);
  doesComboBox_->setModel(doesComboBoxModel_);

  doeLabel_ = new QLabel;
  doeLabel_->setObjectName("doeLabel_");
  doeLayout->addWidget(doeLabel_);

  pageLayout->addWidget(doeBox);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(updateInterestVariables(QStringList)));
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // choose the method
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup(this);

  // Linear regression
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Linear regression"));
  methodGroup_->addButton(buttonToChooseMethod, MetaModelIntroPage::PolynomialRegression);
  methodLayout->addWidget(buttonToChooseMethod);

  // Chaos
  buttonToChooseMethod = new QRadioButton(tr("Functional Chaos"));
  methodGroup_->addButton(buttonToChooseMethod, MetaModelIntroPage::Chaos);
  methodLayout->addWidget(buttonToChooseMethod);

  // Kriging
  buttonToChooseMethod = new QRadioButton(tr("Kriging"));
  methodGroup_->addButton(buttonToChooseMethod, MetaModelIntroPage::Kriging);
  methodLayout->addWidget(buttonToChooseMethod);

  pageLayout->addWidget(methodBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void MetaModelIntroPage::initialize(const Analysis& analysis, QList< DesignOfExperiment > doesList)
{
  SignalBlocker blocker(doesComboBox_);
  // design of experiments
  for (int i = 0; i < doesList.count(); ++i)
  {
    QStandardItem * item = new QStandardItem(doesList[i].getName().c_str());
    item->setData(QVariant::fromValue(doesList[i]));
    doesComboBoxModel_->appendRow(item);
  }
  // interest variables
  interestVariables_ = analysis.getImplementation()->getInterestVariables();

  // update outputsGroupBox_
  updateDesignOfExperiment(0);

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "FunctionalChaosAnalysis")
    methodGroup_->button(MetaModelIntroPage::Chaos)->click();
  else if (analysisName == "KrigingAnalysis")
    methodGroup_->button(MetaModelIntroPage::Kriging)->click();
  else if (analysisName == "PolynomialRegressionAnalysis")
    methodGroup_->button(MetaModelIntroPage::PolynomialRegression)->click();
}


int MetaModelIntroPage::nextId() const
{
  switch (methodGroup_->checkedId())
  {
    case MetaModelIntroPage::Chaos:
      return MetaModelAnalysisWizard::Page_ChaosMethod;
    case MetaModelIntroPage::Kriging:
      return MetaModelAnalysisWizard::Page_KrigingMethod;
    case MetaModelIntroPage::PolynomialRegression:
      return MetaModelAnalysisWizard::Page_PolynomialRegressionMethod;
    default:
      return -1;
  }
}


DesignOfExperiment MetaModelIntroPage::getDesignOfExperiment() const
{
  const int itemRow = doesComboBox_->currentIndex();
  if (itemRow < 0)
    return DesignOfExperiment();
  const QVariant variant = doesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperiment>())
    return variant.value<DesignOfExperiment>();
  return DesignOfExperiment();
}


Description MetaModelIntroPage::getInterestVariables() const
{
  const QStringList outputsList = outputsSelectionGroupBox_->getSelectedOutputsNames();

  Description desc(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  return desc;
}


int MetaModelIntroPage::getMethodId() const
{
  return methodGroup_->checkedId();
}


void MetaModelIntroPage::updateDesignOfExperiment(int index)
{
  Q_ASSERT(doesComboBoxModel_->rowCount());
  if (index < 0)
    return;

  const QVariant variant = doesComboBoxModel_->item(index)->data();
  if (variant.canConvert<DesignOfExperiment>())
  {
    const DesignOfExperiment doe = variant.value<DesignOfExperiment>();
    // sample size label
    OSS labelTextOss;
    labelTextOss << doe.getSample().getSize();
    doeLabel_->setText(tr("Sample size :") + " " + labelTextOss.str().c_str());
    // update outputs list
    outputsSelectionGroupBox_->updateComboBoxModel(doe.getOutputSample().getDescription(), interestVariables_);
    // signal to update krigingPage
    emit designOfExperimentChanged(doe);
  }
}


void MetaModelIntroPage::updateInterestVariables(QStringList outputsList)
{
  interestVariables_ = Description(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    interestVariables_[i] = outputsList[i].toUtf8().constData();
}


bool MetaModelIntroPage::validatePage()
{
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setErrorMessage(tr("At least one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}
}
