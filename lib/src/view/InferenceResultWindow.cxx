//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of InferenceAnalysis
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
#include "otgui/InferenceResultWindow.hxx"

#include "otgui/InferenceAnalysis.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <openturns/VisualTest.hxx>

#include <QHBoxLayout>
#include <QListWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QSplitter>
#include <QScrollArea>

using namespace OT;

namespace OTGUI {

InferenceResultWindow::InferenceResultWindow(AnalysisItem* item)
  : ResultWindow(item)
  , result_(dynamic_cast<InferenceAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  setParameters(item->getAnalysis());
  buildInterface();
}


void InferenceResultWindow::setParameters(const Analysis& analysis)
{
  const InferenceAnalysis * inferenceAnalysis = dynamic_cast<const InferenceAnalysis*>(&*analysis.getImplementation());

  // ParametersWidget
  QStringList namesList;
  namesList << tr("Method");
  namesList << tr("Level");

  QStringList valuesList;
  valuesList << tr("Kolmogorov Smirnov");
  valuesList << QString::number(inferenceAnalysis->getLevel());

  parametersWidget_ = new ParametersWidget(tr("Inference analysis parameters"), namesList, valuesList);
}


void InferenceResultWindow::buildInterface()
{
  setWindowTitle(tr("Inference analysis results"));

  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // table variables
  QStringList variablesNames;
  for (UnsignedInteger i=0; i<result_.getFittingTestResultCollection().getSize(); ++i)
    variablesNames << QString::fromUtf8(result_.getFittingTestResultCollection()[i].getVariableName().c_str());

  QGroupBox * variablesGroupBox = new QGroupBox(tr("Variables"));
  QVBoxLayout * variablesLayoutGroupBox = new QVBoxLayout(variablesGroupBox);

  QListWidget * listVariables = new QListWidget;
  listVariables->addItems(variablesNames);
  connect(listVariables, SIGNAL(currentTextChanged(QString)), this, SLOT(updateInferenceResultWidget(QString)));
  variablesLayoutGroupBox->addWidget(listVariables);

  mainWidget->addWidget(variablesGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tabs for results
  QTabWidget * tabWidget = new QTabWidget;

  inferenceResultWidget_ = new InferenceResultWidget(true, this);

  connect(inferenceResultWidget_, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(inferenceResultWidget_, SIGNAL(graphWindowDeactivated()), this, SIGNAL(graphWindowDeactivated()));

  tabWidget->addTab(inferenceResultWidget_, tr("Summary"));
  tabWidget->addTab(parametersWidget_, tr("Parameters"));
  connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  
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


void InferenceResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  if (indexTab == 0)
    inferenceResultWidget_->showHideGraphConfigurationWidget();
  else
    emit graphWindowDeactivated();
}


void InferenceResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
  {
    if (parametersWidget_->isVisible())
      showHideGraphConfigurationWidget(1);
    else
      showHideGraphConfigurationWidget(0);
  }
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}