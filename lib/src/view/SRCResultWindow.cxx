//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
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
#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/SensitivityResultWidget.hxx"

#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace OTGUI {
  
SRCResultWindow::SRCResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_()
  , outputsListWidget_(0)
  , tabWidget_(0)
{
  const SRCAnalysis * SRCanalysis = dynamic_cast<const SRCAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!SRCanalysis)
    throw InvalidArgumentException(HERE) << "SRCResultWindow: the analysis is not a SRCAnalysis";

  result_ = SRCanalysis->getResult();

  // add warning if the model has not an independent copula
  if (!SRCanalysis->getPhysicalModel().getComposedDistribution().hasIndependentCopula())
    warningMessage_ = tr("The model has not an independent copula, the result could be false.");

  // parameters widget
  setParameters(item->getAnalysis(), tr("Sensitivity analysis parameters"));

  buildInterface();
}


void SRCResultWindow::buildInterface()
{
  setWindowTitle(tr("SRC analysis results"));

  // get output info
  const UnsignedInteger outputDimension = result_.getOutputNames().getSize();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
    outputNames << QString::fromUtf8(result_.getOutputNames()[i].c_str());

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  outputsListWidget_ = new OTguiListWidget;
  outputsListWidget_->addItems(outputNames);
  outputsLayoutGroupBox->addWidget(outputsListWidget_);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // - tab widget
  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));
  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    SensitivityResultWidget * indicesResultWidget = new SensitivityResultWidget(i,
                                                                                result_.getIndices()[i],
                                                                                Point(),
                                                                                result_.getInputNames(),
                                                                                result_.getOutputNames()[i],
                                                                                SensitivityResultWidget::SRC
                                                                               );
    stackedWidget->addWidget(indicesResultWidget);
    connect(indicesResultWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), indicesResultWidget, SLOT(showHideGraphConfigurationWidget(int)));
    connect(this, SIGNAL(stateChanged(int)), indicesResultWidget, SLOT(showHideGraphConfigurationWidget(int)));
  }
  vbox->addWidget(stackedWidget);

  // add a warning (if the model has not an independent copula when doing a SensitivityAnalysis)
  if (!warningMessage_.isEmpty())
  {
    QLabel * warningLabel = new QLabel(QString("<font color=red>%1</font>").arg(warningMessage_));
    vbox->addWidget(warningLabel);
  }
  scrollArea->setWidget(widget);
  tabWidget_->addTab(scrollArea, tr("Indices"));

  // second tab --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  // set widgets
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget_->setCurrentRow(0);
  setWidget(mainWidget);
}


void SRCResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  if (indexTab == 0) // indices
    emit stateChanged(outputsListWidget_->currentRow());
  else
    emit graphWindowDeactivated();
}


void SRCResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}
