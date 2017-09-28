//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SobolAnalysis
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
#include "otgui/SobolResultWindow.hxx"

#include "otgui/SobolAnalysis.hxx"
#include "otgui/SensitivityResultWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/QtTools.hxx"

#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSplitter>

using namespace OT;

namespace OTGUI {
  
SobolResultWindow::SobolResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , warningMessage_("")
{
  const SobolAnalysis * sobolAnalysis = dynamic_cast<const SobolAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!sobolAnalysis)
    throw InvalidArgumentException(HERE) << "SobolResultWindow: the analysis is not a SobolAnalysis";

  result_ = sobolAnalysis->getResult();

  // add warning if the model does not have an independent copula
  if (!sobolAnalysis->getWarningMessage().empty())
    warningMessage_ = tr("The model does not have an independent copula, the result could be false.");

  // parameters widget
  setParameters(item->getAnalysis(), tr("Sensitivity analysis parameters"));

  buildInterface();
}


void SobolResultWindow::buildInterface()
{
  setWindowTitle(tr("Sobol' analysis results"));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputNames().getSize();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  OTguiListWidget * outputsListWidget = new OTguiListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputNames()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // - tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    SensitivityResultWidget * indicesResultWidget = new SensitivityResultWidget(result_.getFirstOrderIndices()[i],
                                                                                result_.getTotalIndices()[i],
                                                                                result_.getInputNames(),
                                                                                result_.getOutputNames()[i],
                                                                                SensitivityResultWidget::Sobol,
                                                                                this);
    stackedWidget->addWidget(indicesResultWidget);
  }
  vbox->addWidget(stackedWidget);

  // add a warning (if the model does not have an independent copula when doing a SensitivityAnalysis)
  if (!warningMessage_.isEmpty())
  {
    QLabel * warningLabel = new QLabel(QString("<font color=red>%1</font>").arg(warningMessage_));
    vbox->addWidget(warningLabel);
  }
  scrollArea->setWidget(widget);
  tabWidget->addTab(scrollArea, tr("Indices"));

  // second tab --------------------------------
  if (result_.getElapsedTime() > 0. && result_.getCallsNumber())
  {
    QWidget * paramWidget = new QWidget;
    QGridLayout * paramWidgetLayout = new QGridLayout(paramWidget);

    // stop criteria
    QStringList namesList;
    namesList << tr("Elapsed time")
              << tr("Number of calls")
              << tr("Coefficient of variation");

    QStringList valuesList;
    valuesList << QString::number(result_.getElapsedTime()) + " s"
               << QString::number(result_.getCallsNumber())
               << QString::number(result_.getCoefficientOfVariation());

    ParametersWidget * parametersWidget = new ParametersWidget(tr("Stop criteria"), namesList, valuesList, true, true);
    paramWidgetLayout->addWidget(parametersWidget);
    paramWidgetLayout->setRowStretch(1, 1);

    tabWidget->addTab(paramWidget, tr("Summary"));
  }

  // third tab --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // set widgets
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);
  setWidget(mainWidget);
}
}
