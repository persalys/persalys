//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
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
#include "otgui/IntroOptimizationPage.hxx"

#include "otgui/OptimizationAnalysis.hxx"
#include "otgui/QtTools.hxx"

#include <openturns/NLopt.hxx>

#include <QVBoxLayout>
#include <QRadioButton>
#include <QScrollArea>
#include <QToolButton>
#include <QDesktopServices>
#include <QUrl> // for qt4

using namespace OT;

namespace OTGUI
{

IntroOptimizationPage::IntroOptimizationPage(QWidget* parent)
  : QWizardPage(parent)
  , outputsSelectionGroupBox_(0)
  , methodGroup_(0)
  , errorMessageLabel_(0)
{
  setTitle(tr("Optimization methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // button to open the NLopt documentation
  if (NLopt::IsAvailable())
  {
    QToolButton * infoButton = new QToolButton;
    infoButton->setIcon(QIcon(":/images/documentinfo.png"));
    infoButton->setToolTip(tr("Open the NLopt documentation"));
    connect(infoButton, SIGNAL(clicked()), this, SLOT(openUrl()));
    pageLayout->addWidget(infoButton);
  }

  // optimization algorithm
  methodGroup_ = new QButtonGroup;

  QVBoxLayout * optimAlgoGroupLayout = new QVBoxLayout;

  for (UnsignedInteger i = 0; i < OptimizationAnalysis::GetSolverNames().getSize(); ++i)
  {
    QRadioButton * methodRadioButton = new QRadioButton(OptimizationAnalysis::GetSolverNames()[i].c_str());
    optimAlgoGroupLayout->addWidget(methodRadioButton);
    methodGroup_->addButton(methodRadioButton, i);
  }

  QGroupBox * optimAlgoGroup = new QGroupBox(tr("Method"));
  // if nlopt: use a scrollarea
  if (OptimizationAnalysis::GetSolverNames().getSize() > 2)
  {
    QWidget * aWidget = new QWidget;
    aWidget->setLayout(optimAlgoGroupLayout);
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(aWidget);
    QVBoxLayout * aLayout = new QVBoxLayout(optimAlgoGroup);
    aLayout->addWidget(scrollArea);
  }
  else
  {
    optimAlgoGroup->setLayout(optimAlgoGroupLayout);
  }
  pageLayout->addWidget(optimAlgoGroup);

  // error message
  errorMessageLabel_ = new TemporaryLabel;

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void IntroOptimizationPage::initialize(const Analysis& analysis)
{
  const OptimizationAnalysis * analysis_ptr = dynamic_cast<const OptimizationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  // method
  const String methodName = analysis_ptr->getSolverName();
  Description names = OptimizationAnalysis::GetSolverNames();
  const Description::const_iterator it = std::find(names.begin(), names.end(), methodName);
  const UnsignedInteger index = it - names.begin();
  methodGroup_->button(index)->click();

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}


Description IntroOptimizationPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}


void IntroOptimizationPage::openUrl()
{
  // open url
  const QString link = "https://nlopt.readthedocs.io/en/latest/NLopt_Algorithms/";
  QDesktopServices::openUrl(QUrl(link));
}


String IntroOptimizationPage::getSolverName() const
{
  return methodGroup_->checkedButton()->text().toStdString();
}


bool IntroOptimizationPage::validatePage()
{
  errorMessageLabel_->setText("");

  if (outputsSelectionGroupBox_->getSelectedOutputsNames().size() != 1)
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("Only one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}
}
