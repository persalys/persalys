//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of InferenceAnalysis
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/InferenceResultWindow.hxx"

#include "otgui/InferenceAnalysis.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>

using namespace OT;

namespace OTGUI
{

InferenceResultWindow::InferenceResultWindow(AnalysisItem* item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_(dynamic_cast<InferenceAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
{
  // parameters widget
  setParameters(item->getAnalysis(), tr("Inference analysis parameters"));

  buildInterface();
}


void InferenceResultWindow::buildInterface()
{
  setWindowTitle(tr("Inference analysis results"));

  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // table variables
  QStringList variablesNames;
  for (UnsignedInteger i = 0; i < result_.getFittingTestResultCollection().getSize(); ++i)
    variablesNames << QString::fromUtf8(result_.getFittingTestResultCollection()[i].getVariableName().c_str());

  QGroupBox * variablesGroupBox = new QGroupBox(tr("Variables"));
  QVBoxLayout * variablesLayoutGroupBox = new QVBoxLayout(variablesGroupBox);

  OTguiListWidget * listVariables = new OTguiListWidget;
  listVariables->addItems(variablesNames);
  connect(listVariables, SIGNAL(currentTextChanged(QString)), this, SLOT(updateInferenceResultWidget(QString)));
  variablesLayoutGroupBox->addWidget(listVariables);

  mainWidget->addWidget(variablesGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tabs for results
  QTabWidget * tabWidget = new QTabWidget;

  inferenceResultWidget_ = new InferenceResultWidget(true, this);

  tabWidget->addTab(inferenceResultWidget_, tr("Summary"));
  tabWidget->addTab(parametersWidget_, tr("Parameters"));

  listVariables->setCurrentRow(0);

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
}


void InferenceResultWindow::updateInferenceResultWidget(QString variableName)
{
  if (!inferenceResultWidget_ || variableName.isEmpty())
    return;

  inferenceResultWidget_->updateDistributionTable(result_, variableName);
}
}
