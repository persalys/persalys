//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of InferenceAnalysis
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
#include "otgui/CopulaInferenceResultWindow.hxx"

#include "otgui/CopulaInferenceAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/CopulaInferenceResultWidget.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>

using namespace OT;

namespace OTGUI {

CopulaInferenceResultWindow::CopulaInferenceResultWindow(AnalysisItem* item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_(dynamic_cast<CopulaInferenceAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , currentResultsTab_(0)
{
  buildInterface();
}


void CopulaInferenceResultWindow::buildInterface()
{
  setWindowTitle(tr("Dependencies inference results"));

  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // list of the set of variables
  QStringList setOfVariablesNames;
  for (UnsignedInteger i=0; i<result_.getCopulaInferenceSetResultCollection().getSize(); ++i)
    setOfVariablesNames << QString::fromUtf8(result_.getCopulaInferenceSetResultCollection()[i].getSetOfVariablesNames().__str__().c_str());

  QGroupBox * variablesGroupBox = new QGroupBox(tr("Variables"));
  QVBoxLayout * variablesLayoutGroupBox = new QVBoxLayout(variablesGroupBox);

  OTguiListWidget * listSetOfVariables = new OTguiListWidget;
  listSetOfVariables->addItems(setOfVariablesNames);
  variablesLayoutGroupBox->addWidget(listSetOfVariables);

  mainWidget->addWidget(variablesGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tabs for results
  // there is a tab for each set of variables
  QTabWidget * tabWidget = new QTabWidget;

  ResizableStackedWidget * inferenceResultStackWidget = new ResizableStackedWidget;
  connect(listSetOfVariables, SIGNAL(currentRowChanged(int)), inferenceResultStackWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i=0; i<result_.getCopulaInferenceSetResultCollection().getSize(); ++i)
  {
    // - find the marginals' indices of the variables of the current set
    const Description setVariables = result_.getCopulaInferenceSetResultCollection()[i].getSetOfVariablesNames();
    Indices indices;
    const Description variablesDOE = result_.getDesignOfExperiment().getSample().getDescription();
    for (UnsignedInteger j=0; j<setVariables.getSize(); ++j)
      for (UnsignedInteger k=0; k<variablesDOE.getSize(); ++k)
        if (variablesDOE[k] == setVariables[j])
          indices.add(k);

    // - create a CopulaInferenceResultWidget
    CopulaInferenceResultWidget * inferenceResultWidget_i =  new CopulaInferenceResultWidget(result_.getCopulaInferenceSetResultCollection()[i],
                                                                                             result_.getDesignOfExperiment().getSample().getMarginal(indices),
                                                                                             true,
                                                                                             this);

    inferenceResultStackWidget->addWidget(inferenceResultWidget_i);

    connect(this, SIGNAL(windowActivated()), inferenceResultWidget_i, SIGNAL(stateChanged()));
    connect(this, SIGNAL(currentResultsTabChanged(int)), inferenceResultWidget_i, SIGNAL(currentResultsTabChangedFromAnother(int)));
    connect(inferenceResultWidget_i, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(inferenceResultWidget_i, SIGNAL(graphWindowDeactivated()), this, SIGNAL(graphWindowDeactivated()));
    connect(inferenceResultWidget_i, SIGNAL(currentResultsTabChanged(int)), this, SLOT(updateCurrentResultsTab(int)));
    connect(listSetOfVariables, SIGNAL(currentRowChanged(int)), inferenceResultWidget_i, SIGNAL(stateChanged()));
  }
  tabWidget->addTab(inferenceResultStackWidget, tr("Summary"));

  listSetOfVariables->setCurrentRow(0);

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
}


void CopulaInferenceResultWindow::updateCurrentResultsTab(int indexTab)
{
  currentResultsTab_ = indexTab;
  emit currentResultsTabChanged(currentResultsTab_);
}


void CopulaInferenceResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    emit windowActivated();
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    emit graphWindowDeactivated();
}
}
