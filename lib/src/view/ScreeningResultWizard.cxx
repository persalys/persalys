//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose a screening result
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
#include "otgui/ScreeningResultWizard.hxx"

#include "otgui/MorrisAnalysis.hxx"
#include "otgui/QtTools.hxx"

#include <QGridLayout>
#include <QHeaderView>
#include <QTableView>

using namespace OT;

namespace OTGUI
{

ScreeningResultWizard::ScreeningResultWizard(const Study& study, const PhysicalModel& model, QWidget* parent)
  : Wizard(parent)
  , study_(study)
  , model_(model)
  , screeningResultsComboBox_(0)
  , variablesComboBox_(0)
  , tableModel_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void ScreeningResultWizard::buildInterface()
{
  setWindowTitle(tr("Screening analyses results"));

  QWizardPage * page = new QWizardPage(this);
  QGridLayout * mainLayout = new QGridLayout(page);

  // choose InferenceAnalysis
  screeningResultsComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("Morris analysis")), 0, 0);
  mainLayout->addWidget(screeningResultsComboBox_ , 0, 1);

  for (UnsignedInteger i = 0; i < study_.getAnalyses().getSize(); ++i)
    if (study_.getAnalyses()[i].getImplementation()->getClassName() == "MorrisAnalysis")
      if (dynamic_cast<MorrisAnalysis*>(study_.getAnalyses()[i].getImplementation().get())->hasValidResult())
        screeningResultsComboBox_->addItem(QString::fromUtf8(study_.getAnalyses()[i].getName().c_str()), (int)i);

  // choose variable
  variablesComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("Variable")), 1, 0);
  mainLayout->addWidget(variablesComboBox_, 1, 1);
  updateVariablesComboBox(0);

  // table
  QTableView * tableView = new QTableView;
  tableView->horizontalHeader()->setStretchLastSection(true);
  mainLayout->addWidget(tableView, 2, 0, 1, 2);
  if (screeningResultsComboBox_->count())
  {
    const int analysisIndex = screeningResultsComboBox_->itemData(screeningResultsComboBox_->currentIndex()).toInt();
    MorrisResult& result(dynamic_cast<MorrisAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get())->getResult());
    tableModel_ = new MorrisResultTableModel(result, variablesComboBox_->currentIndex(), MorrisResultTableModel::Display, tableView);
    tableView->setModel(tableModel_);
    tableView->resizeColumnsToContents();
  }

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_, 3, 0, 1, 2);

  // connections
  connect(variablesComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateTableModel(QString)));
  connect(screeningResultsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateVariablesComboBox(int)));

  addPage(page);
}


void ScreeningResultWizard::updateVariablesComboBox(const int currentAnalysis)
{
  if (!(variablesComboBox_ && screeningResultsComboBox_))
    return;

  // reset
  variablesComboBox_->clear();

  // get variables names
  QStringList variablesNames;
  if (screeningResultsComboBox_->count())
  {
    const int analysisIndex = screeningResultsComboBox_->itemData(currentAnalysis).toInt();
    MorrisResult& result(dynamic_cast<MorrisAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get())->getResult());

    variablesNames = QtOT::DescriptionToStringList(result.getOutputSample().getDescription());
  }
  variablesComboBox_->addItems(variablesNames);
}


void ScreeningResultWizard::updateTableModel(const QString& variableName)
{
  if (!screeningResultsComboBox_ || !variablesComboBox_ || !tableModel_)
    return;

  clearErrorMessage();

  const int analysisIndex = screeningResultsComboBox_->itemData(screeningResultsComboBox_->currentIndex()).toInt();
  MorrisResult& result(dynamic_cast<MorrisAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get())->getResult());

  tableModel_->updateData(result, variablesComboBox_->currentIndex());
}


Indices ScreeningResultWizard::getInputsSelection() const
{
  if (!screeningResultsComboBox_ || !variablesComboBox_)
    return Indices();

  const int analysisIndex = screeningResultsComboBox_->itemData(screeningResultsComboBox_->currentIndex()).toInt();
  MorrisResult& result(dynamic_cast<MorrisAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get())->getResult());

  return result.getInputsSelection(variablesComboBox_->currentIndex());
}


void ScreeningResultWizard::clearErrorMessage()
{
  if (errorMessageLabel_)
    errorMessageLabel_->setText("");
}


bool ScreeningResultWizard::validateCurrentPage()
{
  if (!screeningResultsComboBox_ || !screeningResultsComboBox_->count())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(tr("The current study has not screening analyses results.")));
    return false;
  }

  const int analysisIndex = screeningResultsComboBox_->itemData(screeningResultsComboBox_->currentIndex()).toInt();
  MorrisResult& result(dynamic_cast<MorrisAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get())->getResult());

  if (result.getInputSample().getDimension() != model_.getInputDimension())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(tr("The selected analysis must contain as many results as there are inputs in the model %1.").arg(model_.getInputDimension())));
    return false;
  }

  return QWizard::validateCurrentPage();
}
}
