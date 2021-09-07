//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define simulation reliability analysis
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/ReliabilityIntroPage.hxx"

#include "persalys/ReliabilityAnalysisWizard.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{

ReliabilityIntroPage::ReliabilityIntroPage(QWidget * parent)
  : QWizardPage(parent)
  , limitStatesComboBox_(0)
  , limitStatesComboBoxModel_(0)
  , limitStateLabel_(0)
  , methodGroup_(0)
{
  setTitle(tr("Reliability methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // choose the limit state
  QGroupBox * limitStateBox = new QGroupBox(tr("Limit state"));
  QVBoxLayout * limitStateLayout = new QVBoxLayout(limitStateBox);

  limitStatesComboBox_ = new QComboBox;
  limitStateLayout->addWidget(limitStatesComboBox_);
  connect(limitStatesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLimitStateLabel(int)));
  limitStatesComboBoxModel_ = new QStandardItemModel(limitStatesComboBox_);
  limitStatesComboBox_->setModel(limitStatesComboBoxModel_);

  limitStateLabel_ = new QLabel;
  limitStateLabel_->setObjectName("limitStateLabel_");
  limitStateLayout->addWidget(limitStateLabel_);

  pageLayout->addWidget(limitStateBox);

  // choose the method
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup(this);
  OSS styleText;
  styleText << "QRadioButton { margin-left: " << style()->pixelMetric(QStyle::PM_LayoutLeftMargin) << "px;}";

  // Simulation methods
  QLabel * simuLabel = new QLabel(tr("Simulation methods"));
  simuLabel->setStyleSheet("QLabel {font: bold;}");
  methodLayout->addWidget(simuLabel);

  // - Monte carlo
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, ReliabilityIntroPage::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod->setStyleSheet(styleText.str().c_str());
  // - FORM - IS
  buttonToChooseMethod = new QRadioButton(tr("FORM - Importance sampling"));
  methodGroup_->addButton(buttonToChooseMethod, ReliabilityIntroPage::FORM_IS);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod->setStyleSheet(styleText.str().c_str());

  // Approximation method
  QLabel * approxLabel = new QLabel(tr("Approximation methods"));
  approxLabel->setStyleSheet("QLabel {font: bold;}");
  methodLayout->addWidget(approxLabel);

  // - FORM
  buttonToChooseMethod = new QRadioButton(tr("First Order Reliability Method (FORM)"));
  buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, ReliabilityIntroPage::FORM);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod->setStyleSheet(styleText.str().c_str());

  // - SORM
  buttonToChooseMethod = new QRadioButton(tr("Second Order Reliability Method (SORM)"));
  methodGroup_->addButton(buttonToChooseMethod, ReliabilityIntroPage::SORM);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod->setStyleSheet(styleText.str().c_str());

  connect(methodGroup_, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton * button){emit methodChanged(methodGroup_->id(button));});

  pageLayout->addWidget(methodBox);
}


void ReliabilityIntroPage::initialize(const Analysis& analysis, QList<LimitState> limitStatesList)
{
  // limit state
  for (int i = 0; i < limitStatesList.count(); ++i)
  {
    QStandardItem * item = new QStandardItem(limitStatesList[i].getName().c_str());
    item->setData(QVariant::fromValue(limitStatesList[i]));
    limitStatesComboBoxModel_->appendRow(item);
  }

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "MonteCarloReliabilityAnalysis")
    methodGroup_->button(ReliabilityIntroPage::MonteCarlo)->click();
  else if (analysisName == "FORMImportanceSamplingAnalysis")
    methodGroup_->button(ReliabilityIntroPage::FORM_IS)->click();
  else if (analysisName == "FORMAnalysis")
    methodGroup_->button(ReliabilityIntroPage::FORM)->click();
  else if (analysisName == "SORMAnalysis")
    methodGroup_->button(ReliabilityIntroPage::SORM)->click();
}


int ReliabilityIntroPage::nextId() const
{
  switch (methodGroup_->checkedId())
  {
    case ReliabilityIntroPage::MonteCarlo:
    case ReliabilityIntroPage::FORM_IS:
      return ReliabilityAnalysisWizard::Page_SimuMethod;
    case ReliabilityIntroPage::FORM:
    case ReliabilityIntroPage::SORM:
      return ReliabilityAnalysisWizard::Page_ApproxMethod;
    default:
      return -1;
  }
}


LimitState ReliabilityIntroPage::getLimitState() const
{
  const int itemRow = limitStatesComboBox_->currentIndex();
  if (itemRow < 0)
    return LimitState();

  const QVariant variant = limitStatesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<LimitState>())
    return variant.value<LimitState>();
  return LimitState();
}


int ReliabilityIntroPage::getMethodId() const
{
  return methodGroup_->checkedId();
}


void ReliabilityIntroPage::changeLimitStateLabel(int index)
{
  if (index < 0)
    return;

  const QVariant variant = limitStatesComboBoxModel_->item(index)->data();
  if (variant.canConvert<LimitState>())
  {
    const LimitState limitState = variant.value<LimitState>();
    limitStateLabel_->setText(limitState.__str__().c_str());
  }
}
}
