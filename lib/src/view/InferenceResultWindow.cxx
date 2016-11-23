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

  parametersWidget_ = new ParametersWidget(tr("Inference analysis parameters:"), namesList, valuesList);
}


void InferenceResultWindow::buildInterface()
{
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // table variables
  QStringList variablesNames;
  for (UnsignedInteger i=0; i<result_.getFittingTestResultCollection().getSize(); ++i)
    variablesNames << QString::fromUtf8(result_.getFittingTestResultCollection()[i].getVariableName().c_str());

  QGroupBox * variablesGroupBox = new QGroupBox(tr("Variables"));
  QVBoxLayout * variablesLayoutGroupBox = new QVBoxLayout(variablesGroupBox);

  QListWidget * listVariables = new QListWidget;
  listVariables->addItems(variablesNames);
  variablesLayoutGroupBox->addWidget(listVariables);
  variablesLayoutGroupBox->addStretch();

  mainWidget->addWidget(variablesGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tabs for results
  QTabWidget * tabWidget = new QTabWidget;

  ResizableStackedWidget * summaryStackedWidget = new ResizableStackedWidget;
  ResizableStackedWidget * distStackedWidget = new ResizableStackedWidget;

  for (int i=0; i<variablesNames.size(); ++i)
  {
    // get results
    FittingTestResult fittingTestResult_i = result_.getFittingTestResultCollection()[i];
    Collection<Distribution> listDistributions(fittingTestResult_i.getTestedDistributions());
    Collection<TestResult> listTestResults(fittingTestResult_i.getKolmogorovTestResults());
    NumericalPoint pValues(listDistributions.getSize());
    Interval::BoolCollection acceptationValues(listDistributions.getSize());
    QStringList distributionsNames;
    for (UnsignedInteger j=0; j<listDistributions.getSize(); ++j)
    {
      pValues[j] = listTestResults[j].getPValue();
      acceptationValues[j] = listTestResults[j].getBinaryQualityMeasure();
      distributionsNames << listDistributions[j].getImplementation()->getClassName().c_str();
    }
    // sort indices list
    Indices indices_i(pValues.getSize());
    if (pValues.getSize() > 1)
    {
      indices_i.fill();
      for (int j=(pValues.getSize()-1); j>=0; --j)
      {
        for (int k=1; k<=j; ++k)
        {
          if (pValues[k-1] < pValues[k])
          {
            NumericalScalar temp = pValues[k-1];
            pValues[k-1] = pValues[k];
            pValues[k] = temp;
            UnsignedInteger ind_temp = indices_i[k-1];
            indices_i[k-1] = indices_i[k];
            indices_i[k] = ind_temp;
          }
        }
      }
    }

    // Summary tab ---------------------------------------------------------------------------
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    QWidget * summaryTab = new QWidget;
    QVBoxLayout * summaryTabLayout = new QVBoxLayout(summaryTab);
    summaryTabLayout->setSizeConstraint(QLayout::SetFixedSize);

    // -- table result
    ResizableTableViewWithoutScrollBar * resultTableView = new ResizableTableViewWithoutScrollBar;
    resultTableView->verticalHeader()->hide();
    resultTableView->horizontalHeader()->hide();

    CustomStandardItemModel * resultTableModel = new CustomStandardItemModel(listDistributions.getSize()+2, 3);
    resultTableView->setModel(resultTableModel);

    // --- horizontal header
    resultTableModel->setNotEditableHeaderItem(0, 0, tr("Distributions"));
    resultTableModel->setNotEditableHeaderItem(0, 1, tr("Kolmogorov Smirnov"));
    resultTableModel->setNotEditableHeaderItem(1, 1, tr("p-value"));
    resultTableModel->setNotEditableHeaderItem(1, 2, tr("Acceptation"));
    resultTableView->setSpan(0, 0, 2, 1);

    // --- data
    for (UnsignedInteger j=0; j<listDistributions.getSize(); ++j)
    {
      resultTableModel->setNotEditableItem(j+2, 0, distributionsNames[indices_i[j]]);
      resultTableModel->setNotEditableItem(j+2, 1, pValues[j], 3);
      QString text = acceptationValues[indices_i[j]]? tr("yes") : tr("no");
      QColor color = acceptationValues[indices_i[j]]? Qt::green : QColor();
      resultTableModel->setNotEditableItem(j+2, 2, text, color);
    }

    // --- resize
    resultTableView->resizeColumnsToContents();
    int titleWidth = resultTableView->horizontalHeader()->sectionSize(1);
    // first: clear item at (0,1) because the text is too wide:
    // resizeColumnsToContents takes into account the text of item at (0,1)
    // to resize the column 1, even if there is a setSpan(0, 1, 1, 2)
    resultTableModel->setItem(0, 1, new QStandardItem);
    resultTableView->resizeToContents();
    resultTableModel->setNotEditableHeaderItem(0, 1, tr("Kolmogorov Smirnov"));
    resultTableView->setSpan(0, 1, 1, 2);
    const int subTitlesWidth = resultTableView->horizontalHeader()->sectionSize(1) + resultTableView->horizontalHeader()->sectionSize(2);
    const int widthCorrection = titleWidth - subTitlesWidth;
    if (widthCorrection > 0)
    {
      // correct the table width
      resultTableView->horizontalHeader()->resizeSection(2, resultTableView->horizontalHeader()->sectionSize(2) + widthCorrection);
      resultTableView->setMinimumWidth(resultTableView->minimumWidth() + widthCorrection);
    }

    summaryTabLayout->addWidget(resultTableView);
    summaryTabLayout->addStretch();

    scrollArea->setWidget(summaryTab);
    summaryStackedWidget->addWidget(scrollArea);

    // Distributions tab ---------------------------------------------------------------------
    QWidget * distributionsTab = new QWidget;
    QGridLayout * distributionsTabLayout = new QGridLayout(distributionsTab);

    // -- combobox to choose the distribution
    QLabel * distLabel = new QLabel(tr("Distribution"));
    distributionsTabLayout->addWidget(distLabel, 0, 0);

    QComboBox * listDistributionsComboBox = new QComboBox;
    listDistributionsComboBox->addItems(distributionsNames);
    distributionsTabLayout->addWidget(listDistributionsComboBox, 0, 1, Qt::AlignLeft);
    distributionsTabLayout->setColumnStretch(1, 10);

    // -- stackedwidget
    ResizableStackedWidget * graphResultsStackedWidget = new ResizableStackedWidget;

    for (UnsignedInteger j=0; j<listDistributions.getSize(); ++j)
    {
      QTabWidget * distTabWidget = new QTabWidget;

      // distribution description tab ----------------
      QWidget * distDescriptionTab = new QWidget;
      QGridLayout * distDescriptionTabLayout = new QGridLayout(distDescriptionTab);

      // plot PDF/CDF
      ResizableStackedWidget * pdf_cdfStackedWidget = new ResizableStackedWidget;
      // --- pdf
      PlotWidget * pdfPlot = new PlotWidget;
      pdfPlot->plotHistogram(fittingTestResult_i.getValues());
      pdfPlot->plotPDFCurve(listDistributions[j]);
      pdfPlot->setTitle(tr("PDF") + " " + distributionsNames[j]);
      pdf_cdfStackedWidget->addWidget(pdfPlot);
      // --- cdf
      PlotWidget * cdfPlot = new PlotWidget;
      cdfPlot->plotHistogram(fittingTestResult_i.getValues(), 1);
      cdfPlot->plotCDFCurve(listDistributions[j]);
      cdfPlot->setTitle(tr("CDF") + " " + distributionsNames[j]);
      pdf_cdfStackedWidget->addWidget(cdfPlot);
      // --- GraphConfigurationWidget
      QVector<PlotWidget*> listpdf_cdfPlot;
      listpdf_cdfPlot.append(pdfPlot);
      listpdf_cdfPlot.append(cdfPlot);
      GraphConfigurationWidget * pdf_cdfPlotGraphConfigWidget = new GraphConfigurationWidget(listpdf_cdfPlot, QStringList(), QStringList(), GraphConfigurationWidget::PDF, this);
      pdf_cdfPlotGraphConfigWidget->hide();
      connect(pdfPlot, SIGNAL(visibilityChanged(bool)), pdf_cdfPlotGraphConfigWidget, SLOT(plotVisibilityChanged(bool)));
      connect(cdfPlot, SIGNAL(visibilityChanged(bool)), pdf_cdfPlotGraphConfigWidget, SLOT(plotVisibilityChanged(bool)));
      connect(pdf_cdfPlotGraphConfigWidget, SIGNAL(currentPlotChanged(int)), pdf_cdfStackedWidget, SLOT(setCurrentIndex(int)));
      connect(pdf_cdfPlotGraphConfigWidget, SIGNAL(visibilityChanged(QWidget*,bool)), this, SLOT(showHideGraphConfigurationWidget(QWidget*,bool)));
      distDescriptionTabLayout->addWidget(pdf_cdfStackedWidget, 0, 0);

      // distribution parameters
      QGroupBox * paramGroupBox = new QGroupBox(tr("Distribution parameters"));
      QVBoxLayout * paramGroupBoxLayout = new QVBoxLayout(paramGroupBox);

      ResizableTableViewWithoutScrollBar * distTableView = new ResizableTableViewWithoutScrollBar;
      distTableView->horizontalHeader()->hide();
      distTableView->verticalHeader()->hide();

      CustomStandardItemModel * distTableModel = new CustomStandardItemModel(4, 2);
      distTableView->setModel(distTableModel);

      // --- vertical header
      distTableModel->setNotEditableHeaderItem(0, 0, tr("Mean"));
      distTableModel->setNotEditableHeaderItem(1, 0, tr("Standard deviation"));
      distTableModel->setNotEditableHeaderItem(2, 0, tr("Skewness"));
      distTableModel->setNotEditableHeaderItem(3, 0, tr("Kurtosis"));
      if (listDistributions[j].getImplementation()->getClassName() != "Normal") // mean and std already displayed
      {
        UnsignedInteger nbParamDesc = listDistributions[j].getParameterDescription().getSize();
        if (listDistributions[j].getImplementation()->getClassName() == "Student")
          nbParamDesc = 1; // display only nu (mean and std already displayed)

        for (UnsignedInteger k=0; k<nbParamDesc; ++k)
          distTableModel->setNotEditableHeaderItem(3+k, 0, listDistributions[j].getParameterDescription()[k].c_str());
      }

      // -- parameters
      distTableModel->setNotEditableItem(0, 1, listDistributions[j].getMean()[0]);
      distTableModel->setNotEditableItem(1, 1, listDistributions[j].getStandardDeviation()[0]);
      try
      {
        distTableModel->setNotEditableItem(2, 1, listDistributions[j].getSkewness()[0]);
      }
      catch (std::exception & ex)
      {
        distTableModel->setNotEditableItem(2, 1, tr("-"));
      }
      try
      {
        distTableModel->setNotEditableItem(3, 1, listDistributions[j].getKurtosis()[0]);
      }
      catch (std::exception & ex)
      {
        distTableModel->setNotEditableItem(3, 1, tr("-"));
      }

      if (listDistributions[j].getImplementation()->getClassName() != "Normal") // mean and std already displayed
      {
        UnsignedInteger nbParamVal = listDistributions[j].getParameterDescription().getSize();
        if (listDistributions[j].getImplementation()->getClassName() == "Student")
          nbParamVal = 1; // display only nu (mean and std already displayed)

        for (UnsignedInteger k=0; k<nbParamVal; ++k)
          distTableModel->setNotEditableItem(3+k, 1, listDistributions[j].getParameter()[k]);
      }

      // --- resize
      distTableView->resizeToContents();

      paramGroupBoxLayout->addWidget(distTableView);
      paramGroupBoxLayout->addStretch();

      distDescriptionTabLayout->addWidget(paramGroupBox, 0, 1, Qt::AlignVCenter);

      distTabWidget->addTab(distDescriptionTab, tr("Description"));

      // QQ plot tab ---------------------------------
      QWidget * qqPlotTab = new QWidget;
      QHBoxLayout * qqPlotTabLayout = new QHBoxLayout(qqPlotTab);

      // --- qq plot
      PlotWidget * qqPlot = new PlotWidget;
      Graph qqPlotGraph(VisualTest::DrawQQplot(fittingTestResult_i.getValues(), listDistributions[j]));
      qqPlot->plotCurve(qqPlotGraph.getDrawable(1).getData(), QPen(Qt::blue, 5), QwtPlotCurve::Dots);
      qqPlot->plotCurve(qqPlotGraph.getDrawable(0).getData());
      qqPlot->setTitle(tr("Q-Q Plot") + " " + distributionsNames[j]);
      qqPlot->setAxisTitle(QwtPlot::yLeft, tr("Data quantiles"));
      qqPlot->setAxisTitle(QwtPlot::xBottom, tr("%1 theoretical quantiles").arg(distributionsNames[j]));
      QVector<PlotWidget*> listPlot;
      listPlot.append(qqPlot);
      GraphConfigurationWidget * qqPlotGraphConfigWidget = new GraphConfigurationWidget(listPlot, QStringList(), QStringList(), GraphConfigurationWidget::NoType, this);
      qqPlotGraphConfigWidget->hide();
      connect(qqPlot, SIGNAL(visibilityChanged(bool)), qqPlotGraphConfigWidget, SLOT(plotVisibilityChanged(bool)));
      connect(qqPlotGraphConfigWidget, SIGNAL(visibilityChanged(QWidget*,bool)), this, SLOT(showHideGraphConfigurationWidget(QWidget*,bool)));
      qqPlotTabLayout->addWidget(qqPlot);

      distTabWidget->addTab(qqPlotTab, tr("Q-Q Plot"));

      graphResultsStackedWidget->addWidget(distTabWidget);
    }
    distributionsTabLayout->addWidget(graphResultsStackedWidget, 1, 0, 1, 2);
    connect(listDistributionsComboBox, SIGNAL(currentIndexChanged(int)), graphResultsStackedWidget, SLOT(setCurrentIndex(int)));

    distStackedWidget->addWidget(distributionsTab);
  }

  tabWidget->addTab(summaryStackedWidget, tr("Summary"));
  tabWidget->addTab(distStackedWidget, tr("Distributions"));
  tabWidget->addTab(parametersWidget_, tr("Parameters"));
  
  connect(listVariables, SIGNAL(currentRowChanged(int)), summaryStackedWidget, SLOT(setCurrentIndex(int)));
  connect(listVariables, SIGNAL(currentRowChanged(int)), distStackedWidget, SLOT(setCurrentIndex(int)));
  listVariables->setCurrentRow(0);

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
}


void InferenceResultWindow::showHideGraphConfigurationWidget(QWidget* widget, bool visibility)
{
  if (visibility)
    emit graphWindowActivated(widget);
  else
    emit graphWindowDeactivated();
}


void InferenceResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    emit graphWindowDeactivated();
}
}