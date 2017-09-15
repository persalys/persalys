//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
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
#include "otgui/FunctionalChaosResultWindow.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/MetaModelValidationWidget.hxx"
#include "otgui/SensitivityResultWidget.hxx"
#include "otgui/QtTools.hxx"

#include <QSplitter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

FunctionalChaosResultWindow::FunctionalChaosResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , maxDegree_(0)
  , sparse_(false)
  , errorMessage_(item->getAnalysis().getErrorMessage().c_str())
{
  const FunctionalChaosAnalysis * chaos(dynamic_cast<const FunctionalChaosAnalysis*>(item->getAnalysis().getImplementation().get()));
  if (!chaos)
    throw InvalidArgumentException(HERE) << "FunctionalChaosResultWindow: the analysis is not a FunctionalChaosAnalysis";

  result_ = chaos->getResult();
  maxDegree_ = chaos->getChaosDegree();
  sparse_ = chaos->getSparseChaos();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Metamodel creation parameters"));

  buildInterface();
}


void FunctionalChaosResultWindow::buildInterface()
{
  setWindowTitle(tr("Functional chaos results"));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputSample().getDescription().getSize();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  OTguiListWidget * outputsListWidget = new OTguiListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputSample().getDescription()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // - tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * metaModelPlotLayout = new QVBoxLayout(tab);

  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(result_.getMetaModelOutputSample().getMarginal(i),
                                                                                 result_.getOutputSample().getMarginal(i),
                                                                                 result_.getFunctionalChaosResult().getResiduals()[i],
                                                                                 result_.getFunctionalChaosResult().getRelativeErrors()[i],
                                                                                 tr("Relative error"),
                                                                                 this);

    plotsStackedWidget->addWidget(validationWidget);
  }
  metaModelPlotLayout->addWidget(plotsStackedWidget);

  tabWidget->addTab(tab, tr("Metamodel"));

  // second tab : MOMENTS --------------------------------
  if (result_.getMean().getSize() == nbOutputs && result_.getVariance().getSize() == nbOutputs)
  {
    QWidget * summaryWidget = new QWidget;
    QGridLayout * summaryWidgetLayout = new QGridLayout(summaryWidget);

    // moments estimates
    QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimates"));
    QVBoxLayout * momentsGroupBoxLayout = new QVBoxLayout(momentsGroupBox);
    ResizableStackedWidget * momentsStackedWidget = new ResizableStackedWidget;

    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      ResizableTableViewWithoutScrollBar * momentsEstimationsTableView = new ResizableTableViewWithoutScrollBar;
      momentsEstimationsTableView->horizontalHeader()->hide();
      momentsEstimationsTableView->verticalHeader()->hide();
      CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(3, 2, momentsEstimationsTableView);
      momentsEstimationsTableView->setModel(momentsEstimationsTable);
      // - vertical header
      momentsEstimationsTable->setNotEditableHeaderItem(0, 0, tr("Estimate"));
      momentsEstimationsTable->setNotEditableHeaderItem(1, 0, tr("Mean"));
      momentsEstimationsTable->setNotEditableHeaderItem(2, 0, tr("Variance"));
      momentsEstimationsTable->setNotEditableHeaderItem(3, 0, tr("Standard deviation"));
      // - horizontal header
      momentsEstimationsTable->setNotEditableHeaderItem(0, 1, tr("Value"));
      // - moments values
      momentsEstimationsTable->setNotEditableItem(1, 1, result_.getMean()[outputIndex]);
      momentsEstimationsTable->setNotEditableItem(2, 1, result_.getVariance()[outputIndex]);
      momentsEstimationsTable->setNotEditableItem(3, 1, std::sqrt(result_.getVariance()[outputIndex]));

      momentsEstimationsTableView->resizeToContents();

      momentsStackedWidget->addWidget(momentsEstimationsTableView);
    }
    momentsGroupBoxLayout->addWidget(momentsStackedWidget);

    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), momentsStackedWidget, SLOT(setCurrentIndex(int)));

    summaryWidgetLayout->addWidget(momentsGroupBox);

    // chaos result
    QGroupBox * basisGroupBox = new QGroupBox(tr("Polynomial basis"));
    QVBoxLayout * basisGroupBoxLayout = new QVBoxLayout(basisGroupBox);
    ResizableStackedWidget * basisStackedWidget = new ResizableStackedWidget;

    if (result_.getFunctionalChaosResult().getCoefficients().getDimension() != nbOutputs)
      qDebug() << "Error: FunctionalChaosResultWindow chaos coefficients sample has not a dimension equal to the number of outputs\n";

    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      // parameters names
      QStringList namesList;
      namesList << tr("Dimension")
                << tr("Maximum degree");
      namesList << (sparse_? tr("Full basis size") : tr("Basis size"));

      // parameters values
      const UnsignedInteger dim = result_.getFunctionalChaosResult().getDistribution().getDimension();
      const UnsignedInteger maxSize = FunctionalChaosAnalysis::BinomialCoefficient(dim + maxDegree_, maxDegree_);

      QStringList valuesList;
      valuesList << QString::number(dim)
                 << QString::number(maxDegree_)
                 << QString::number(maxSize);

      if (sparse_)
      {
        namesList << tr("Basis size");

        UnsignedInteger notNullCoefCounter = 0;
        for (UnsignedInteger coefIndex = 0; coefIndex < result_.getFunctionalChaosResult().getCoefficients().getSize(); ++coefIndex)
          if (result_.getFunctionalChaosResult().getCoefficients()[coefIndex][outputIndex] != 0.0)
            ++notNullCoefCounter;
        valuesList << QString::number(notNullCoefCounter);
      }

      // table view
      ParametersTableView * basisTableView = new ParametersTableView(namesList, valuesList, true, true);
      basisStackedWidget->addWidget(basisTableView);
    }
    basisGroupBoxLayout->addWidget(basisStackedWidget);

    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), basisStackedWidget, SLOT(setCurrentIndex(int)));

    summaryWidgetLayout->addWidget(basisGroupBox);
//     summaryWidgetLayout->addStretch();
    summaryWidgetLayout->setRowStretch(2, 1);

    tabWidget->addTab(summaryWidget, tr("Summary"));
  }
  else
  {
    if (!errorMessage_.isEmpty())
    {
      QWidget * summaryWidget = new QWidget;
      QVBoxLayout * summaryWidgetLayout = new QVBoxLayout(summaryWidget);
      QLabel * errorLabel = new QLabel(errorMessage_);
      summaryWidgetLayout->addWidget(errorLabel);
      summaryWidgetLayout->addStretch();
      tabWidget->addTab(summaryWidget, tr("Summary"));
    }
  }

  // third tab : SOBOL INDICES --------------------------------
  if (result_.getSobolResult().getOutputNames().getSize() == nbOutputs)
  {
    QScrollArea * sobolScrollArea = new QScrollArea;
    sobolScrollArea->setWidgetResizable(true);
    QWidget * widget = new QWidget;
    QVBoxLayout * vbox = new QVBoxLayout(widget);
    ResizableStackedWidget * sobolStackedWidget = new ResizableStackedWidget;
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), sobolStackedWidget, SLOT(setCurrentIndex(int)));
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      SensitivityResultWidget * sobolResultWidget = new SensitivityResultWidget(result_.getSobolResult().getFirstOrderIndices()[i],
                                                                                result_.getSobolResult().getTotalIndices()[i],
                                                                                result_.getSobolResult().getInputNames(),
                                                                                result_.getSobolResult().getOutputNames()[i],
                                                                                SensitivityResultWidget::Sobol,
                                                                                this);
      sobolStackedWidget->addWidget(sobolResultWidget);
    }
    vbox->addWidget(sobolStackedWidget);
    sobolScrollArea->setWidget(widget);
    tabWidget->addTab(sobolScrollArea, tr("Sobol indices"));
  }
  else
  {
    if (!errorMessage_.isEmpty())
    {
      QWidget * indicesWidget = new QWidget;
      QVBoxLayout * indicesWidgetLayout = new QVBoxLayout(indicesWidget);
      QLabel * errorLabel = new QLabel(errorMessage_);
      indicesWidgetLayout->addWidget(errorLabel);
      indicesWidgetLayout->addStretch();
      tabWidget->addTab(indicesWidget, tr("Sobol indices"));
    }
  }

  // fourth tab : VALIDATION --------------------------------
  if (result_.getMetaModelOutputSampleLeaveOneOut().getSize())
  {
    QTabWidget * validationTabWidget = new QTabWidget;

    tab = new QWidget;
    metaModelPlotLayout = new QVBoxLayout(tab);

    ResizableStackedWidget * plotsLOOStackedWidget = new ResizableStackedWidget;
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsLOOStackedWidget, SLOT(setCurrentIndex(int)));
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(result_.getMetaModelOutputSampleLeaveOneOut().getMarginal(i),
                                                                                   result_.getOutputSample().getMarginal(i),
                                                                                   result_.getErrorQ2LeaveOneOut()[i],
                                                                                   result_.getQ2LeaveOneOut()[i],
                                                                                   tr("Q2"),
                                                                                   this);
      plotsLOOStackedWidget->addWidget(validationWidget);
    }
    metaModelPlotLayout->addWidget(plotsLOOStackedWidget);

    validationTabWidget->addTab(plotsLOOStackedWidget, tr("Leave-one-out"));
    tabWidget->addTab(validationTabWidget, tr("Validation"));
  }

  // fourth/fifth tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // set widgets
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);
  setWidget(mainWidget);
}
}
