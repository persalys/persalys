//                                               -*- C++ -*-
/**
 *  @brief QWidget for copula representation
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
#include "otgui/CopulaParametersTabWidget.hxx"

#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/TranslationManager.hxx"

#include <openturns/NormalCopula.hxx>
#include <openturns/VisualTest.hxx>
#include <openturns/Combinations.hxx>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace OTGUI
{

CopulaParametersTabWidget::CopulaParametersTabWidget(const Distribution& distribution,
    const Sample& sample,
    const DataSample::SampleCollection& kendallPlotData,
    QWidget* parent)
  : QTabWidget(parent)
  , distribution_(distribution)
  , sample_(sample)
  , kendallPlotData_(kendallPlotData)
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
  const QString distName = TranslationManager::GetTranslatedDistributionName(distributionName);

  const QPen pen(Qt::blue, 2);

  // tab PDF/CDF
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  ResizableStackedWidget * pdf_StackedWidget = new ResizableStackedWidget;

  // -- plots creation
  // create a PDF and a CDF for each pair of variables
  QVector<PlotWidget*> listPlot;
  const int size = sample_.getSize();
  const Sample sampleRanks(sample_.rank() / size);

  for (UnsignedInteger i = 0; i < distribution_.getDimension(); ++i)
  {
    for (UnsignedInteger j = 0; j < distribution_.getDimension(); ++j)
    {
      if (i != j)
      {
        Indices marginals(2);
        marginals[0] = i;
        marginals[1] = j;

        //pdf
        PlotWidget * pdfPlot = new PlotWidget(tr("copulaPDF"));
        //  use rank of the Sample to have the points in [0, 1]*[0, 1]
        pdfPlot->plotCurve(sampleRanks.getMarginal(marginals), pen, QwtPlotCurve::Dots);
        pdfPlot->plotContour(distribution_.getMarginal(marginals));
        pdfPlot->setTitle(tr("PDF") + " " + tr("%1 copula").arg(distName));
        pdfPlot->setAxisTitle(QwtPlot::xBottom, variablesNames[i]);
        pdfPlot->setAxisTitle(QwtPlot::yLeft, variablesNames[j]);
        pdf_StackedWidget->addWidget(pdfPlot);
        listPlot.append(pdfPlot);

        // cdf
        PlotWidget * cdfPlot = new PlotWidget(tr("copulaCDF"));
        cdfPlot->plotContour(distribution_.getMarginal(marginals), false);
        cdfPlot->setTitle(tr("CDF") + " " + tr("%1 copula").arg(distName));
        cdfPlot->setAxisTitle(QwtPlot::xBottom, variablesNames[i]);
        cdfPlot->setAxisTitle(QwtPlot::yLeft, variablesNames[j]);
        pdf_StackedWidget->addWidget(cdfPlot);
        listPlot.append(cdfPlot);
      }
    }
  }

  // -- GraphConfigurationWidget
  GraphConfigurationWidget * pdf_cdfPlotSettingWidget = new GraphConfigurationWidget(listPlot,
      variablesNames,
      QStringList(),
      GraphConfigurationWidget::Copula,
      this);
  plotWidget->setDockWidget(pdf_cdfPlotSettingWidget);
  connect(pdf_cdfPlotSettingWidget, SIGNAL(currentPlotChanged(int)), pdf_StackedWidget, SLOT(setCurrentIndex(int)));

  plotWidgetLayout->addWidget(pdf_StackedWidget);
  addTab(plotWidget, tr("PDF/CDF"));

  // tab Kendall plot
  plotWidget = new WidgetBoundToDockWidget(this);
  plotWidgetLayout = new QVBoxLayout(plotWidget);

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
    kendallPlot->setTitle(tr("Kendall plot") + ": " + tr("%1 copula").arg(distName));
    kendallPlot->setAxisTitle(QwtPlot::xBottom, tr("%1 copula").arg(distName));
    const QString pairNames = QString::fromUtf8(kendallPlotData_[i].getDescription()[0].c_str());
    kendallPlot->setAxisTitle(QwtPlot::yLeft, tr("Data") + " " + pairNames);
    variablesPairsNames << pairNames;

    kendall_StackedWidget->addWidget(kendallPlot);
    listKendallPlots.append(kendallPlot);
  }

  // -- GraphConfigurationWidget
  GraphConfigurationWidget * kendallPlotSettingWidget = new GraphConfigurationWidget(listKendallPlots,
      variablesPairsNames,
      QStringList(),
      GraphConfigurationWidget::Kendall,
      this);
  plotWidget->setDockWidget(kendallPlotSettingWidget);
  connect(kendallPlotSettingWidget, SIGNAL(currentPlotChanged(int)), kendall_StackedWidget, SLOT(setCurrentIndex(int)));

  plotWidgetLayout->addWidget(kendall_StackedWidget);
  addTab(plotWidget, tr("Kendall plot"));

  // tab Parameters
  QWidget * paramWidget = new QWidget;
  QVBoxLayout * paramGroupBoxLayout = new QVBoxLayout(paramWidget);

  // -- show parameters values
  // --- table creation
  ResizableTableViewWithoutScrollBar * distParamTableView = new ResizableTableViewWithoutScrollBar;
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
    distParamTableModel->setNotEditableHeaderItem(0, 0, distribution_.getParameterDescription()[0].c_str());
    distParamTableModel->setNotEditableItem(0, 1, distribution_.getParameter()[0]);
  }
  // --- resize
  distParamTableView->resizeToContents();

  // add tab
  paramGroupBoxLayout->addWidget(distParamTableView);
  paramGroupBoxLayout->addStretch();
  addTab(paramWidget, tr("Parameters"));
}
}
