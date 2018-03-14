//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of data analysis
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/DataAnalysisResultWindow.hxx"

#include "otgui/DataAnalysis.hxx"
#include "otgui/QtTools.hxx"

#include <openturns/Normal.hxx>

#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI
{

DataAnalysisResultWindow::DataAnalysisResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  if (!dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()))
    throw InvalidArgumentException (HERE) << "Can NOT build the DataAnalysisResultWindow: The analysis of the item is not valid";

  initialize(item);

  buildInterface();

  setWindowTitle(tr("Data analysis results"));
}


void DataAnalysisResultWindow::initialize(AnalysisItem* item)
{
  DataAnalysis analysis(*dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()));

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();
}


void DataAnalysisResultWindow::fillTabWidget()
{
  // tab: Summary
  addSummaryTab();
  if (designOfExperiment_.getSample().getSize() > 1)
  {
    // tab: PDF/CDF
    addPDF_CDFTab();
    // tab: box plots
    addBoxPlotTab();
    // tab: correlation
    addDependencyTab();
  }
  bool canUseParaview = false;
#ifdef OTGUI_HAVE_PARAVIEW
  if (OTguiSubWindow::HaveOpenGL32())
  {
    addParaviewWidgetsTabs();
    canUseParaview = true;
  }
#endif
  if (!canUseParaview)
  {
    // tab: Table --------------------------------
    addTableTab();
    if (designOfExperiment_.getSample().getDimension() > 1 && designOfExperiment_.getSample().getSize() > 1)
    {
      // tab: plot C
      addPlotMatrixTab();
      // tab: scatter plots
      addScatterPlotsTab();
    }
  }

  // tab: Parameters --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(updateVariablesListVisibility(int)));
}


void DataAnalysisResultWindow::addDependencyTab()
{
  // compute correlation
  CorrelationMatrix C(designOfExperiment_.getSample().computeSpearmanCorrelation());

  const UnsignedInteger dim = designOfExperiment_.getSample().getDimension();

  // consider only significantly non-zero correlations
  const double alpha = 0.05;
  const double epsilon = Normal().computeQuantile(1 - alpha)[0] / std::sqrt(designOfExperiment_.getSample().getSize() - 1);

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QGroupBox * gpBox = new QGroupBox(tr("Spearman's matrix estimate"));
  QHBoxLayout * gpBoxLayout = new QHBoxLayout(gpBox);

  // table widget
  QTableWidget * table = new QTableWidget(dim, dim);
  table->setFocusPolicy(Qt::NoFocus);
  table->setSelectionMode(QAbstractItemView::NoSelection);

  for (UnsignedInteger i = 0; i < dim; ++i)
  {
    for (UnsignedInteger j = 0; j < dim; ++j)
    {
      QTableWidgetItem * item = new QTableWidgetItem;
      item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

      if (i != j)
      {
        if (std::abs(C(i, j)) > epsilon)
        {
          if (C(i, j) < -0.7)
            item->setBackgroundColor("#7caef4");  //dark blue
          else if (C(i, j) >= -0.7 && C(i, j) < -0.3)
            item->setBackgroundColor("#b0cef8");  //blue
          else if (C(i, j) >= -0.3 && C(i, j) < 0.)
            item->setBackgroundColor("#e4eefc");  //light blue
          else if (C(i, j) > 0. && C(i, j) <= 0.3)
            item->setBackgroundColor("#fadec3");  //light orange
          else if (C(i, j) > 0.3 && C(i, j) <= 0.7)
            item->setBackgroundColor("#f4b87c");  //orange
          else if (C(i, j) > 0.7)
            item->setBackgroundColor("#ee9235");  //dark orange
        }
      }
      else
      {
        item->setBackgroundColor(Qt::black);
      }
      table->setItem(i, j, item);
    }
  }
  QStringList headers(QtOT::DescriptionToStringList(designOfExperiment_.getSample().getDescription()));
  table->setHorizontalHeaderLabels(headers);
  table->setVerticalHeaderLabels(headers);
  table->resizeColumnsToContents();
  gpBoxLayout->addWidget(table);

  // color bar
  QStringList labels;
  labels << "ρ > 0.7"
         << "0.3 < ρ ≤ 0.7"
         << "ε < ρ ≤ 0.3"
         << "-ε ≤ ρ ≤ ε"
         << "-0.3 ≤ ρ < -ε"
         << "-0.7 ≤ ρ < -0.3"
         << "ρ < -0.7";
  QStringList colors;
  colors << "#ee9235" << "#f4b87c" << "#fadec3" << "#ffffff" << "#e4eefc" << "#b0cef8" << "#7caef4";

  QTableWidget * colorTable = new QTableWidget(labels.size(), 1);
  colorTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  colorTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  colorTable->setFocusPolicy(Qt::NoFocus);
  colorTable->setSelectionMode(QAbstractItemView::NoSelection);
  colorTable->setShowGrid(false);
  for (int i = 0; i < labels.size(); ++i)
  {
    QPixmap px(20, 20);
    px.fill(colors[i]);
    QTableWidgetItem * item = new QTableWidgetItem(px, labels[i]);
    colorTable->setItem(i, 0, item);
  }
  colorTable->setHorizontalHeaderLabels(QStringList() << tr("Spearman's\ncoefficient"));
  colorTable->resizeColumnsToContents();
  colorTable->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
  const int w = colorTable->horizontalHeader()->length();
  int x1, y1, x2, y2;
  colorTable->getContentsMargins(&x1, &y1, &x2, &y2);
  colorTable->setFixedWidth(w + x1 + x2);
  colorTable->verticalHeader()->hide();

  gpBoxLayout->addWidget(colorTable, 0, Qt::AlignTop);

  mainLayout->addWidget(gpBox);

  tabWidget_->addTab(mainWidget, tr("Dependency"));
}
}
