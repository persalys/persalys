//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/SRCResultWindow.hxx"

#include "persalys/SRCAnalysis.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/SensitivityResultWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/ParametersTableView.hxx"

#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSplitter>

using namespace OT;

namespace PERSALYS
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

  widgetLayout->addWidget(new TitleLabel(tr("Standardized Regression Coefficients"), "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#srcresult"));

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

  // - indices graph and table
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    QWidget * indicesWidget = new QWidget;
    QVBoxLayout * indicesLayout = new QVBoxLayout(indicesWidget);

    // table
    if (result_.getR2().getSize() == nbOutputs)
    {
      QStringList namesList;
      namesList << tr("Elapsed time")
                  << tr("Number of calls")
                  << tr("R2");

      QStringList valuesList;
      valuesList << QtOT::FormatDuration(result_.getElapsedTime())
                 << QString::number(result_.getCallsNumber())
                 << QString::number(result_.getR2()[i]);
      ParametersTableView * basisTableView = new ParametersTableView(namesList, valuesList, true, true);
      indicesLayout->addWidget(basisTableView);
    }

    // indices graph and table
    Interval indicesInterval = result_.getIndicesInterval().getSize() == nbOutputs? result_.getIndicesInterval()[i] : Interval();
    Interval signedIndicesInterval = result_.getSignedIndicesInterval().getSize() == nbOutputs? result_.getSignedIndicesInterval()[i] : Interval();
    SensitivityResultWidget * indicesResultWidget = new SensitivityResultWidget(result_.getIndices()[i],
        indicesInterval,
        result_.getSignedIndices().getSize() == nbOutputs ? result_.getSignedIndices()[i] : Point(),
        signedIndicesInterval,
        result_.getInputNames(),
        result_.getOutputNames()[i],
        SensitivityResultWidget::SRC,
        this);
    indicesLayout->addWidget(indicesResultWidget);
    stackedWidget->addWidget(indicesWidget);
  }
  vbox->addWidget(stackedWidget);

  // add a warning (if the model does not have an independent copula when doing a SensitivityAnalysis)
  if (!warningMessage_.isEmpty())
  {
    QLabel * warningLabel = new QLabel(QString("<font color=red>%1</font>").arg(warningMessage_));
    vbox->addWidget(warningLabel);
  }
  vbox->setContentsMargins(0, 0, 0, 0);
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
