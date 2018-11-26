//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
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
#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/SensitivityResultWidget.hxx"
#include "otgui/QtTools.hxx"

#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSplitter>

using namespace OT;

namespace OTGUI
{

SRCResultWindow::SRCResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
{
  const SRCAnalysis * SRCanalysis = dynamic_cast<const SRCAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!SRCanalysis)
    throw InvalidArgumentException(HERE) << "SRCResultWindow: the analysis is not a SRCAnalysis";

  result_ = SRCanalysis->getResult();

  // add warning if the model does not have an independent copula
  if (!SRCanalysis->getWarningMessage().empty())
    warningMessage_ = tr("The model does not have an independent copula, the result could be false.");

  // parameters widget
  setParameters(item->getAnalysis(), tr("Sensitivity analysis parameters"));

  buildInterface();
}


void SRCResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Standardized Regression Coefficients")));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputNames().getSize();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
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
    SensitivityResultWidget * indicesResultWidget = new SensitivityResultWidget(result_.getIndices()[i],
        Interval(),
        Point(),
        Interval(),
        result_.getInputNames(),
        result_.getOutputNames()[i],
        SensitivityResultWidget::SRC,
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
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  // set widgets
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);

  widgetLayout->addWidget(mainWidget, 1);
}
}
