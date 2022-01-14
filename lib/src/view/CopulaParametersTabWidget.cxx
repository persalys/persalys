//                                               -*- C++ -*-
/**
 *  @brief QWidget for copula representation
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/CopulaParametersTabWidget.hxx"

#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/DocumentationToolButton.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/ContourPlot.hxx"

#include <openturns/NormalCopula.hxx>
#include <openturns/VisualTest.hxx>
#include <openturns/Combinations.hxx>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QDesktopServices>

using namespace OT;

namespace PERSALYS
{

CopulaParametersTabWidget::CopulaParametersTabWidget(const Distribution& distribution,
    const Sample& sample,
    const ProcessSample & kendallPlotData,
    const bool displaySetting,
    QWidget* parent)
  : QTabWidget(parent)
  , distribution_(distribution)
  , sample_(sample)
  , kendallPlotData_(kendallPlotData)
  , displaySetting_(displaySetting)
{
  buildInterface();
}


void CopulaParametersTabWidget::buildInterface()
{
  if (distribution_.getDimension() < 2)
    return;

  // get variables names
  QStringList variablesNames = QtOT::DescriptionToStringList(distribution_.getDescription());

  // get distribution name
  String distributionName = distribution_.getImplementation()->getClassName();
  distributionName = distributionName.substr(0, distributionName.find("Copula"));
  const QString distName = TranslationManager::GetTranslatedCopulaName(distributionName);

  const QPen pen(Qt::blue, 2);

  // tab PDF/CDF
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  ResizableStackedWidget * pdf_StackedWidget = new ResizableStackedWidget;

  // -- plots creation
  // create a PDF and a CDF for each pair of variables
  QVector<PlotWidget*> listPlot;
  const int size = sample_.getSize();
  const Sample sampleRanks(sample_.rank() / size);

  // if NormalCopula of dimension > 2: Display copula for each pair of variables
  // else: display copula for one pair of variables
  const UnsignedInteger dim = distribution_.getDimension() == 2 ? 1 : distribution_.getDimension();

  for (UnsignedInteger i = 0; i < dim; ++i)
  {
    for (UnsignedInteger j = 0; j < distribution_.getDimension(); ++j)
    {
      if (i != j)
      {
        Indices marginals(2);
        marginals[0] = i;
        marginals[1] = j;

        //pdf
        ContourPlot * pdfPlot = new ContourPlot(distribution_.getMarginal(marginals), true);
        //  use rank of the Sample to have the points in [0, 1]*[0, 1]
        pdfPlot->plotCurve(sampleRanks.getMarginal(marginals), pen, QwtPlotCurve::Dots);
        pdfPlot->setTitle(tr("PDF") + ": " + distName);
        pdfPlot->setAxisTitle(QwtPlot::xBottom, variablesNames[i]);
        pdfPlot->setAxisTitle(QwtPlot::yLeft, variablesNames[j]);
        pdf_StackedWidget->addWidget(pdfPlot);
        listPlot.append(pdfPlot);

        // cdf
        ContourPlot * cdfPlot = new ContourPlot(distribution_.getMarginal(marginals), false);
        cdfPlot->setTitle(tr("CDF") + ": " + distName);
        cdfPlot->setAxisTitle(QwtPlot::xBottom, variablesNames[i]);
        cdfPlot->setAxisTitle(QwtPlot::yLeft, variablesNames[j]);
        pdf_StackedWidget->addWidget(cdfPlot);
        listPlot.append(cdfPlot);
      }
    }
  }

  PDFGraphSetting * pdf_cdfPlotSettingWidget = 0;
  if (displaySetting_)
  {
    // -- GraphConfigurationWidget
    pdf_cdfPlotSettingWidget = new PDFGraphSetting(listPlot, dim == 1 ? QStringList() : variablesNames, PDFGraphSetting::Copula, this);
    connect(pdf_cdfPlotSettingWidget, SIGNAL(currentPlotChanged(int)), pdf_StackedWidget, SLOT(setCurrentIndex(int)));
  }

  scrollArea->setWidget(new WidgetBoundToDockWidget(pdf_StackedWidget, pdf_cdfPlotSettingWidget, this));
  addTab(scrollArea, tr("PDF/CDF"));

  // tab Kendall plot
  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  DocumentationToolButton * infoKendallPlotButton = new DocumentationToolButton("theory/data_analysis/graphical_fitting_test.html", FileTools::docOT);
  plotWidgetLayout->addWidget(infoKendallPlotButton);

  ResizableStackedWidget * kendall_StackedWidget = new ResizableStackedWidget;

  // -- plots creation
  // create a Kendall plot for each pair of variables
  QVector<PlotWidget*> listKendallPlots;

  QStringList variablesPairsNames;
  // diagonal
  Sample dataDiagonal(0, 2);
  dataDiagonal.add(Point(2, 0.0));
  dataDiagonal.add(Point(2, 1.0));

  for (UnsignedInteger i = 0; i < kendallPlotData_.getSize(); ++i)
  {
    //pdf
    PlotWidget * kendallPlot = new PlotWidget(tr("kendallPlot"));

    kendallPlot->plotCurve(kendallPlotData_[i], pen);
    kendallPlot->plotCurve(dataDiagonal);
    kendallPlot->setTitle(tr("Kendall plot") + ": " + distName);
    kendallPlot->setAxisTitle(QwtPlot::xBottom, tr("%1 copula").arg(distName));
    const QString pairNames = QString::fromUtf8(kendallPlotData_[i].getDescription()[0].c_str());
    kendallPlot->setAxisTitle(QwtPlot::yLeft, tr("Data") + " " + pairNames);
    variablesPairsNames << pairNames;

    kendall_StackedWidget->addWidget(kendallPlot);
    listKendallPlots.append(kendallPlot);
  }

  if (displaySetting_ && kendallPlotData_.getSize())
  {
    // -- GraphConfigurationWidget
    SimpleGraphSetting * kendallPlotSettingWidget = new SimpleGraphSetting(listKendallPlots, variablesPairsNames, this);
    plotWidget->setDockWidget(kendallPlotSettingWidget);
    connect(kendallPlotSettingWidget, SIGNAL(currentPlotChanged(int)), kendall_StackedWidget, SLOT(setCurrentIndex(int)));
  }

  plotWidgetLayout->addWidget(kendall_StackedWidget);
  scrollArea->setWidget(plotWidget);
  addTab(scrollArea, tr("Kendall plot"));

  // tab Parameters
  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QWidget * paramWidget = new QWidget;
  QVBoxLayout * paramGroupBoxLayout = new QVBoxLayout(paramWidget);

  // -- show parameters values
  // --- table creation
  CopyableTableView * distParamTableView = new CopyableTableView;
  CustomStandardItemModel * distParamTableModel = new CustomStandardItemModel(0, 0, distParamTableView);
  distParamTableView->setModel(distParamTableModel);

  // -- fill table
  // ---- if Normal copula : show spearman correlation table
  if (distribution_.getImplementation()->getClassName() == "NormalCopula")
  {
    QLabel * titleLabel = new QLabel(tr("Spearman's rank"));
    paramGroupBoxLayout->addWidget(titleLabel);

    distParamTableModel->setHorizontalHeaderLabels(variablesNames);
    distParamTableModel->setVerticalHeaderLabels(variablesNames);

    NormalCopula copula = *dynamic_cast<NormalCopula*>(distribution_.getImplementation().get());

    for (UnsignedInteger i = 0; i < distribution_.getDimension(); ++i)
      for (UnsignedInteger j = 0; j < distribution_.getDimension(); ++j)
        distParamTableModel->setNotEditableItem(i, j, copula.getSpearmanCorrelation()(i, j));
  }
  // ---- if NOT Normal copula : show the copula parameter value
  else
  {
    distParamTableView->horizontalHeader()->hide();
    distParamTableView->verticalHeader()->hide();
    const QString paramName = TranslationManager::GetTranslatedDistributionParameterName(distribution_.getParameterDescription()[0]);
    distParamTableModel->setNotEditableHeaderItem(0, 0, paramName);
    distParamTableModel->setNotEditableItem(0, 1, distribution_.getParameter()[0]);
  }
  // --- resize
  distParamTableView->resizeToContents();

  // add tab
  paramGroupBoxLayout->addWidget(distParamTableView);

  // button to open the OT documentation
  DocumentationToolButton * infoButton = new DocumentationToolButton("", FileTools::docOT);
  connect(infoButton, SIGNAL(clicked()), this, SLOT(openUrl()));
  paramGroupBoxLayout->addWidget(infoButton);

  paramGroupBoxLayout->addStretch();
  scrollArea->setWidget(paramWidget);
  addTab(scrollArea, tr("Parameters"));
}


void CopulaParametersTabWidget::openUrl()
{
  const String distName = distribution_.getImplementation()->getClassName();
  const QString link = FileTools::OpenTURNSUrlLink + "user_manual/_generated/openturns." + QString(distName.c_str()) + ".html";

  // open url
  QDesktopServices::openUrl(QUrl(link));
}


void CopulaParametersTabWidget::setCurrentIndexWithoutSignal(int index)
{
  SignalBlocker(this);
  setCurrentIndex(index);
}
}
