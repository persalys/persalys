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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/DataAnalysisResultWindow.hxx"

#include "otgui/DataAnalysis.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/CustomStandardItemModel.hxx"

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
}


void DataAnalysisResultWindow::initialize(AnalysisItem* item)
{
  DataAnalysis analysis(*dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()));

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();
  C_ = designOfExperiment_.getSample().computeSpearmanCorrelation();

  titleLabel_->setText(tr("Data analysis"));
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
    addDependenceTab();
  }
  bool canUseParaview = false;
#ifdef OTGUI_HAVE_PARAVIEW
  if (SubWindow::HaveOpenGL32())
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


void DataAnalysisResultWindow::addDependenceTab()
{
  const UnsignedInteger dim = designOfExperiment_.getSample().getDimension();

  // consider only significantly non-zero correlations
  const double alpha = 0.05;
  const double epsilon = Normal().computeQuantile(1 - alpha)[0] / std::sqrt(designOfExperiment_.getSample().getSize() - 1);

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QGroupBox * gpBox = new QGroupBox(tr("Spearman's matrix estimate"));
  QHBoxLayout * gpBoxLayout = new QHBoxLayout(gpBox);

  // table widget
  tableView_ = new ExportableTableView;

  CustomStandardItemModel * tableModel = new CustomStandardItemModel(dim, dim, tableView_);

  for (UnsignedInteger i = 0; i < dim; ++i)
  {
    for (UnsignedInteger j = 0; j < dim; ++j)
    {
      QStandardItem * item = new QStandardItem;
      item->setData(C_(i, j), Qt::UserRole + 10);
      item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);

      if (i != j)
      {
        item->setText(QString::number(C_(i, j), 'g', 3));
        // set Background color
        if (std::abs(C_(i, j)) > epsilon)
        {
          if (C_(i, j) < -0.7)
            item->setBackground(QBrush("#7caef4"));  //dark blue
          else if (C_(i, j) >= -0.7 && C_(i, j) < -0.3)
            item->setBackground(QBrush("#b0cef8"));  //blue
          else if (C_(i, j) >= -0.3 && C_(i, j) < 0.)
            item->setBackground(QBrush("#e4eefc"));  //light blue
          else if (C_(i, j) > 0. && C_(i, j) <= 0.3)
            item->setBackground(QBrush("#fadec3"));  //light orange
          else if (C_(i, j) > 0.3 && C_(i, j) <= 0.7)
            item->setBackground(QBrush("#f4b87c"));  //orange
          else if (C_(i, j) > 0.7)
            item->setBackground(QBrush("#ee9235"));  //dark orange
        }
      }
      else
      {
        item->setText("1.");
        item->setBackground(Qt::black);
      }
      tableModel->setItem(i, j, item);
    }
  }
  QStringList headers(QtOT::DescriptionToStringList(designOfExperiment_.getSample().getDescription()));
  tableModel->setHorizontalHeaderLabels(headers);
  tableModel->setVerticalHeaderLabels(headers);

  tableView_->setModel(tableModel);
  tableView_->resizeColumnsToContents();
  gpBoxLayout->addWidget(tableView_);

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
    colorTable->setItem(i, 0, new QTableWidgetItem(px, labels[i]));
  }
  colorTable->setHorizontalHeaderLabels(QStringList() << tr("Spearman's coefficient"));
  colorTable->resizeColumnsToContents();
  colorTable->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
  const int w = colorTable->horizontalHeader()->length();
  const int h = colorTable->verticalHeader()->length() + colorTable->horizontalHeader()->height();
  int x1, y1, x2, y2;
  colorTable->getContentsMargins(&x1, &y1, &x2, &y2);
  colorTable->setFixedSize(w + x1 + x2, h + y1 + y2);
  colorTable->verticalHeader()->hide();

  gpBoxLayout->addWidget(colorTable, 0, Qt::AlignTop);

  mainLayout->addWidget(gpBox);

  tabWidget_->addTab(mainWidget, tr("Dependence"));
}
}
