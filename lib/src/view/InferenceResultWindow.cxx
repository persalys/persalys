//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of InferenceAnalysis
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
#include "persalys/InferenceResultWindow.hxx"

#include "persalys/InferenceAnalysis.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>

using namespace OT;

namespace PERSALYS
{

InferenceResultWindow::InferenceResultWindow(AnalysisItem* item, QWidget * parent)
  : ResultWindow(item, parent)
  , level_(0)
  , result_()
{
  InferenceAnalysis * analysis = dynamic_cast<InferenceAnalysis*>(item->getAnalysis().getImplementation().get());
  Q_ASSERT(analysis);
  level_ = analysis->getLevel();
  result_ = analysis->getResult();

  buildInterface();
}


void InferenceResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Marginals inference"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#marginalsinferenceresult"));

  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // table variables
  QStringList variablesNames;
  for (UnsignedInteger i = 0; i < result_.getFittingTestResultCollection().getSize(); ++i)
    variablesNames << QString::fromUtf8(result_.getFittingTestResultCollection()[i].getVariableName().c_str());

  QGroupBox * variablesGroupBox = new QGroupBox(tr("Variables"));
  QVBoxLayout * variablesLayoutGroupBox = new QVBoxLayout(variablesGroupBox);

  VariablesListWidget * listVariables = new VariablesListWidget;
  listVariables->addItems(variablesNames);
  connect(listVariables, SIGNAL(currentTextChanged(QString)), this, SLOT(updateInferenceResultWidget(QString)));
  variablesLayoutGroupBox->addWidget(listVariables);

  mainWidget->addWidget(variablesGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tabs for results
  QTabWidget * tabWidget = new QTabWidget;

  inferenceResultWidget_ = new InferenceResultWidget(true, this);

  tabWidget->addTab(inferenceResultWidget_, tr("Summary"));

  listVariables->setCurrentRow(0);

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  widgetLayout->addWidget(mainWidget, 1);
}


void InferenceResultWindow::updateInferenceResultWidget(QString variableName)
{
  if (!inferenceResultWidget_ || variableName.isEmpty())
    return;

  inferenceResultWidget_->updateDistributionTable(level_, result_, variableName);
}
}
