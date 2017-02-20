//                                               -*- C++ -*-
/**
 *  @brief QWidget for copula representation
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
#include "otgui/CopulaParametersTabWidget.hxx"

#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <openturns/NormalCopula.hxx>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace OTGUI {

CopulaParametersTabWidget::CopulaParametersTabWidget(const Distribution& distribution,
                                                     const NumericalSample& sample,
                                                     QWidget* parent)
  : QTabWidget(parent)
  , distribution_(distribution)
  , sample_(sample)
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
    variablesNames << distribution_.getDescription()[i].c_str();

  // tab PDF/CDF
  ResizableStackedWidget * pdf_StackedWidget = new ResizableStackedWidget;

  // -- plots creation
  // create a PDF and a CDF for each pair of variables
  QVector<PlotWidget*> listPlot;
  for (UnsignedInteger i=0; i<distribution_.getDimension(); ++i)
  {
    for (UnsignedInteger j=0; j<distribution_.getDimension(); ++j)
    {
      if (i != j)
      {
        String distributionName = distribution_.getImplementation()->getClassName();
        distributionName = distributionName.substr(0, distributionName.find("Copula"));

        Indices marginals(2);
        marginals[0] = i;
        marginals[1] = j;

        //pdf
        PlotWidget * plotWidget = new PlotWidget;
        //  use rank of the NumericalSample to have the points in [0, 1]*[0, 1]
        const QPen pen(PlotWidget::DefaultHistogramColor, 2);
        const int size = sample_.getSize();
        plotWidget->plotCurve(sample_.getMarginal(marginals).rank() / size, pen, QwtPlotCurve::Dots);
        plotWidget->plotContour(distribution_.getMarginal(marginals));
        plotWidget->setTitle(tr("PDF") + " " + distributionName.c_str() + " copula");
        plotWidget->setAxisTitle(QwtPlot::xBottom, sample_.getDescription()[i].c_str());
        plotWidget->setAxisTitle(QwtPlot::yLeft, sample_.getDescription()[j].c_str());
        pdf_StackedWidget->addWidget(plotWidget);
        listPlot.append(plotWidget);

        // cdf
        plotWidget = new PlotWidget;
        plotWidget->plotContour(distribution_.getMarginal(marginals), false);
        plotWidget->setTitle(tr("CDF") + " " + distributionName.c_str() + " copula");
        plotWidget->setAxisTitle(QwtPlot::xBottom, sample_.getDescription()[i].c_str());
        plotWidget->setAxisTitle(QwtPlot::yLeft, sample_.getDescription()[j].c_str());
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
  if (!index)
  {
    if (pdf_cdfPlotGraphConfigWidget_)
      if (!pdf_cdfPlotGraphConfigWidget_->isVisible())
        emit graphWindowActivated(pdf_cdfPlotGraphConfigWidget_);
  }
  // parameters tab
  else
    emit graphWindowDeactivated();
}
}