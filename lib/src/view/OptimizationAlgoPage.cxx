//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
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
#include "persalys/OptimizationAlgoPage.hxx"

#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/DocumentationToolButton.hxx"

#include <openturns/NLopt.hxx>

#include <QVBoxLayout>
#include <QRadioButton>
#include <QScrollArea>

using namespace OT;

namespace PERSALYS
{

OptimizationAlgoPage::OptimizationAlgoPage(QWidget* parent)
  : QWizardPage(parent)
  , outputsSelectionGroupBox_(0)
  , methodGroup_(0)
  , errorMessageLabel_(0)
  , pageLayout_(0)
{
  setTitle(tr("Optimization methods"));
  pageLayout_ = new QVBoxLayout(this);
}


void OptimizationAlgoPage::initialize(OptimizationAnalysis& analysis)
{
  QLayoutItem *child;
  while ((child = pageLayout_->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(false, this);
  pageLayout_->addWidget(outputsSelectionGroupBox_);

  // button to open OT documentation
  DocumentationToolButton * infoButton = new DocumentationToolButton("user_manual/optimization.html", FileTools::docOT);
  pageLayout_->addWidget(infoButton);

  // optimization algorithm
  methodGroup_ = new QButtonGroup(this);

  QVBoxLayout * optimAlgoGroupLayout = new QVBoxLayout;
  Description solverNames = OptimizationAnalysis::GetSolverNames(analysis_ptr->getBounds());

  for (UnsignedInteger i = 0; i < solverNames.getSize(); ++i)
  {
    QRadioButton * methodRadioButton = new QRadioButton;
    methodRadioButton->setText(solverNames[i].c_str());
    optimAlgoGroupLayout->addWidget(methodRadioButton);
    methodGroup_->addButton(methodRadioButton, i);
  }

  QGroupBox * optimAlgoGroup = new QGroupBox(tr("Method"));
  // if nlopt: use a scrollarea
  if (solverNames.getSize() > 2)
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
  pageLayout_->addWidget(optimAlgoGroup);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

  pageLayout_->addStretch();
  pageLayout_->addWidget(errorMessageLabel_);


  // method
  const String methodName = analysis_ptr->getSolverName();
  const Description::const_iterator it = std::find(solverNames.begin(), solverNames.end(), methodName);
  UnsignedInteger index = 0;
  //if algo is no longer compatible default to Cobyla
  if(it!=solverNames.end())
    index = it - solverNames.begin();
  else {
    LOGWARN(OSS() << "Optimization problem has changed and " << methodName << " algorithm is no longer available... Using Cobyla as default.");
    index = std::find(solverNames.begin(), solverNames.end(), "Cobyla") - solverNames.begin();
  }

  methodGroup_->button(index)->click();

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}

}


Description OptimizationAlgoPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}


String OptimizationAlgoPage::getSolverName() const
{
  return methodGroup_->checkedButton()->text().toStdString();
}


bool OptimizationAlgoPage::validatePage()
{
  if (outputsSelectionGroupBox_->getSelectedOutputsNames().size() != 1)
  {
    errorMessageLabel_->setErrorMessage(tr("Only one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}
}
