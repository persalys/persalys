//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose an inference result
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/InferenceResultWizard.hxx"

#include "otgui/InferenceAnalysis.hxx"

#include <QGridLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QGroupBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

using namespace OT;

namespace OTGUI {

InferenceResultWizard::InferenceResultWizard(const OTStudy& otStudy, QWidget* parent)
  : OTguiWizard(parent)
  , otStudy_(otStudy)
  , inferenceResultsComboBox_(0)
  , variablesComboBox_(0)
  , inferenceResultWidget_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void InferenceResultWizard::buildInterface()
{
  setWindowTitle(tr("Inference analyses results"));

  QWizardPage * page = new QWizardPage(this);
  QGridLayout * mainLayout = new QGridLayout(page);

  // choose InferenceAnalysis
  inferenceResultsComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("Inference analysis")), 0, 0);
  mainLayout->addWidget(inferenceResultsComboBox_ , 0, 1);

  QStringList inferenceAnalyses;
  for (UnsignedInteger i=0; i<otStudy_.getAnalyses().getSize(); ++i)
  {
    if (otStudy_.getAnalyses()[i].getImplementation()->getClassName() == "InferenceAnalysis")
    {
      inferenceAnalyses << otStudy_.getAnalyses()[i].getName().c_str();
      inferenceResultsComboBox_->addItem(otStudy_.getAnalyses()[i].getName().c_str(), (int)i);
    }
  }
  // choose variable
  variablesComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("Variable")), 1, 0);
  mainLayout->addWidget(variablesComboBox_, 1, 1);

  // widget with tables
  inferenceResultWidget_ = new InferenceResultWidget(false, this);
  mainLayout->addWidget(inferenceResultWidget_, 2, 0, 1, 2);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_, 3, 0, 1, 2);

  // update tables
  connect(variablesComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateInferenceResultWidget(QString)));
  connect(inferenceResultsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateVariablesComboBox(int)));
  updateVariablesComboBox(0);

  addPage(page);
}


void InferenceResultWizard::updateVariablesComboBox(int currentAnalysis)
{
  if (!(errorMessageLabel_ && variablesComboBox_ && inferenceResultsComboBox_))
    return;

  // reset
  errorMessageLabel_->setText("");
  variablesComboBox_->clear();

  // get variables names
  QStringList variablesNames;
  if (inferenceResultsComboBox_->count())
  {
    const int analysisIndex = inferenceResultsComboBox_->itemData(currentAnalysis).toInt();
    InferenceResult result(dynamic_cast<InferenceAnalysis*>(&*otStudy_.getAnalyses()[analysisIndex].getImplementation())->getResult());
    Collection< FittingTestResult > fittingTestResultCollection(result.getFittingTestResultCollection());

    for (UnsignedInteger i=0; i<fittingTestResultCollection.getSize(); ++i)
      variablesNames << fittingTestResultCollection[i].getVariableName().c_str();
  }
  variablesComboBox_->addItems(variablesNames);
}


void InferenceResultWizard::updateInferenceResultWidget(QString variableName)
{
  if (!(inferenceResultsComboBox_ && inferenceResultWidget_))
    return;

  const int analysisIndex = inferenceResultsComboBox_->itemData(inferenceResultsComboBox_->currentIndex()).toInt();
  InferenceResult result(dynamic_cast<InferenceAnalysis*>(&*otStudy_.getAnalyses()[analysisIndex].getImplementation())->getResult());

  inferenceResultWidget_->updateDistributionTable(result, variableName);
}


Distribution InferenceResultWizard::getDistribution() const
{
  return inferenceResultWidget_->getDistribution();
}


bool InferenceResultWizard::validateCurrentPage()
{
  if (!inferenceResultsComboBox_->count())
    return false;

  return QWizard::validateCurrentPage();
}
}