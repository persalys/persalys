//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the design of experiments and the method of metamodel analysis
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
#include "otgui/IntroMetaModelPage.hxx"

#include "otgui/MetaModelAnalysisWizard.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace OTGUI
{

IntroMetaModelPage::IntroMetaModelPage(QWidget* parent)
  : QWizardPage(parent)
  , doesComboBox_(0)
  , doesComboBoxModel_(0)
  , doeLabel_(0)
  , outputsSelectionGroupBox_(0)
  , methodGroup_(0)
  , errorMessageLabel_(0)
  , interestVariables_()
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

  // Chaos
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Functional Chaos"));
  methodGroup_->addButton(buttonToChooseMethod, MetaModelAnalysisWizard::chaos);
  methodLayout->addWidget(buttonToChooseMethod);

  // Kriging
  buttonToChooseMethod = new QRadioButton(tr("Kriging"));
  methodGroup_->addButton(buttonToChooseMethod, MetaModelAnalysisWizard::kriging);
  methodLayout->addWidget(buttonToChooseMethod);

  pageLayout->addWidget(methodBox);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void IntroMetaModelPage::initialize(const Analysis& analysis, QList< DesignOfExperiment > doesList)
{
  // design of experiments
  for (int i = 0; i < doesList.count(); ++i)
  {
    QStandardItem * item = new QStandardItem(doesList[i].getName().c_str());
    item->setData(qVariantFromValue(doesList[i]));
    doesComboBoxModel_->appendRow(item);
  }
  // interest variables
  interestVariables_ = analysis.getImplementation()->getInterestVariables();

  // update outputsGroupBox_
  updateDesignOfExperiment(0);

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "FunctionalChaosAnalysis")
    methodGroup_->button(MetaModelAnalysisWizard::chaos)->click();
  else if (analysisName == "KrigingAnalysis")
    methodGroup_->button(MetaModelAnalysisWizard::kriging)->click();
}


int IntroMetaModelPage::nextId() const
{
  switch (methodGroup_->checkedId())
  {
    case MetaModelAnalysisWizard::chaos:
      return MetaModelAnalysisWizard::Page_ChaosMethod;
    case MetaModelAnalysisWizard::kriging:
      return MetaModelAnalysisWizard::Page_KrigingMethod;
    default:
      return -1;
  }
}


DesignOfExperiment IntroMetaModelPage::getDesignOfExperiment() const
{
  const int itemRow = doesComboBox_->currentIndex();
  if (itemRow < 0)
    return 0;
  const QVariant variant = doesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperiment>())
    return variant.value<DesignOfExperiment>();
  return DesignOfExperiment();
}


Description IntroMetaModelPage::getInterestVariables() const
{
  const QStringList outputsList = outputsSelectionGroupBox_->getSelectedOutputsNames();

  Description desc(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  return desc;
}


void IntroMetaModelPage::updateDesignOfExperiment(int index)
{
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


void IntroMetaModelPage::updateInterestVariables(QStringList outputsList)
{
  interestVariables_ = Description(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    interestVariables_[i] = outputsList[i].toUtf8().constData();
}


bool IntroMetaModelPage::validatePage()
{
  errorMessageLabel_->setText("");

  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(tr("At least one output must be selected")));
    return false;
  }
  return QWizardPage::validatePage();
}
}
