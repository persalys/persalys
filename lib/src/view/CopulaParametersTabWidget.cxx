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

#include <openturns/NormalCopula.hxx>
#include <openturns/VisualTest.hxx>
#include <openturns/Combinations.hxx>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace OTGUI {

CopulaParametersTabWidget::CopulaParametersTabWidget(const Distribution& distribution,
                                                     const NumericalSample& sample,
                                                     const DataSample::NumericalSampleCollection& kendallPlotData,
                                                     QWidget* parent)
  : QTabWidget(parent)
  , distribution_(distribution)
  , sample_(sample)
  , kendallPlotData_(kendallPlotData)
  , pdf_cdfPlotGraphConfigWidget_(0)
{
  buildInterface();
}


void CopulaParametersTabWidget::buildInterface()
{
  if (distribution_.getDimension() < 2)
    return;

  // get variables names
  QStringList variablesNames;
  for (UnsignedInteger i=0; i<distribution_.getDimension(); ++i)
    variablesNames << QString::fromUtf8(distribution_.getDescription()[i].c_str());

  // get distribution name
  String distributionName = distribution_.getImplementation()->getClassName();
  distributionName = distributionName.substr(0, distributionName.find("Copula"));

  const QPen pen(PlotWidget::DefaultHistogramColor, 2);

  // tab PDF/CDF
  ResizableStackedWidget * pdf_StackedWidget = new ResizableStackedWidget;

  // -- plots creation
  // create a PDF and a CDF for each pair of variables
  QVector<PlotWidget*> listPlot;
  const int size = sample_.getSize();
  NumericalSample sampleRanks(sample_.rank() / size);
  for (UnsignedInteger i=0; i<distribution_.getDimension(); ++i)
  {
    for (UnsignedInteger j=0; j<distribution_.getDimension(); ++j)
    {
      if (i != j)
      {
        Indices marginals(2);
        marginals[0] = i;
        marginals[1] = j;

        //pdf
        PlotWidget * plotWidget = new PlotWidget;
        //  use rank of the NumericalSample to have the points in [0, 1]*[0, 1]
        plotWidget->plotCurve(sampleRanks.getMarginal(marginals), pen, QwtPlotCurve::Dots);
        plotWidget->plotContour(distribution_.getMarginal(marginals));
        plotWidget->setTitle(tr("PDF") + " " + distributionName.c_str() + " copula");
        plotWidget->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(sample_.getDescription()[i].c_str()));
        plotWidget->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(sample_.getDescription()[j].c_str()));
        pdf_StackedWidget->addWidget(plotWidget);
        listPlot.append(plotWidget);

        // cdf
        plotWidget = new PlotWidget;
        plotWidget->plotContour(distribution_.getMarginal(marginals), false);
        plotWidget->setTitle(tr("CDF") + " " + distributionName.c_str() + " copula");
        plotWidget->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(sample_.getDescription()[i].c_str()));
        plotWidget->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(sample_.getDescription()[j].c_str()));
        pdf_StackedWidget->addWidget(plotWidget);
        listPlot.append(plotWidget);
      }
    }
  }

  // -- GraphConfigurationWidget
  pdf_cdfPlotGraphConfigWidget_ = new GraphConfigurationWidget(listPlot, variablesNames, QStringList(), GraphConfigurationWidget::Copula);
  connect(pdf_cdfPlotGraphConfigWidget_, SIGNAL(currentPlotChanged(int)), pdf_StackedWidget, SLOT(setCurrentIndex(int)));
  connect(this, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  addTab(pdf_StackedWidget, tr("PDF/CDF"));

  // tab Kendall plot
  ResizableStackedWidget * kendall_StackedWidget = new ResizableStackedWidget;

  // -- plots creation
  // create a Kendall plot for each pair of variables
  QVector<PlotWidget*> listKendallPlots;

  QStringList variablesPairsNames;
  // diagonal
  NumericalSample dataDiagonal(0, 2);
  dataDiagonal.add(NumericalPoint(2, 0.0));
  dataDiagonal.add(NumericalPoint(2, 1.0));

  for (UnsignedInteger i=0; i<kendallPlotData_.getSize(); ++i)
  {
    //pdf
    PlotWidget * plotWidget = new PlotWidget;

    plotWidget->plotCurve(kendallPlotData_[i], pen);
    plotWidget->plotCurve(dataDiagonal);
    plotWidget->setTitle(tr("Kendall plot") + ": " + distributionName.c_str() + " copula");
    plotWidget->setAxisTitle(QwtPlot::xBottom, tr("%1 copula").arg(distributionName.c_str()));
    QString pairNames = QString::fromUtf8(kendallPlotData_[i].getDescription()[0].c_str());
    plotWidget->setAxisTitle(QwtPlot::yLeft, tr("Data") + " " + pairNames);
    variablesPairsNames << pairNames;

    kendall_StackedWidget->addWidget(plotWidget);
    listKendallPlots.append(plotWidget);
  }

  // -- GraphConfigurationWidget
  kendallPlotGraphConfigWidget_ = new GraphConfigurationWidget(listKendallPlots, variablesPairsNames, QStringList(), GraphConfigurationWidget::Kendall);
  connect(kendallPlotGraphConfigWidget_, SIGNAL(currentPlotChanged(int)), kendall_StackedWidget, SLOT(setCurrentIndex(int)));

  addTab(kendall_StackedWidget, tr("Kendall plot"));

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

    NormalCopula copula = *dynamic_cast<NormalCopula*>(&*distribution_.getImplementation());

    for (UnsignedInteger i=0; i<distribution_.getDimension(); ++i)
      for (UnsignedInteger j=0; j<distribution_.getDimension(); ++j)
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


void CopulaParametersTabWidget::stateChanged()
{
  // if this widget is the current widget
  if (isVisible())
    showHideGraphConfigurationWidget(currentIndex());
}


void CopulaParametersTabWidget::showHideGraphConfigurationWidget(int index)
{
  // PDF/CDF tab
  if (index == 0)
  {
    if (pdf_cdfPlotGraphConfigWidget_)
      if (!pdf_cdfPlotGraphConfigWidget_->isVisible())
        emit graphWindowActivated(pdf_cdfPlotGraphConfigWidget_);
  }
  else if (index == 1)
  {
    if (kendallPlotGraphConfigWidget_)
      if (!kendallPlotGraphConfigWidget_->isVisible())
        emit graphWindowActivated(kendallPlotGraphConfigWidget_);
  }
  // parameters tab
  else
    emit graphWindowDeactivated();
}
}