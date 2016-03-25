//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of TaylorExpansionsMomentsAnalysis
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
#include "otgui/TaylorExpansionsMomentsResultWindow.hxx"
#include "otgui/DataTableWidget.hxx"
#include "otgui/TaylorExpansionsMomentsAnalysis.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

TaylorExpansionsMomentsResultWindow::TaylorExpansionsMomentsResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<TaylorExpansionsMomentsAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void TaylorExpansionsMomentsResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // -- output name --
  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBox_ = new QComboBox;
  QStringList items;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    items << result_.getOutputNames()[i].c_str();
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimatesTable(int)));
  headLayout->addWidget(outputsComboBox_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // -- results --

  // moments estimation
  QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimate"));
  QVBoxLayout * momentsVbox = new QVBoxLayout(momentsGroupBox);
  momentsEstimationsTable_ = new NotEditableTableWidget(5, 2);
  momentsEstimationsTable_->horizontalHeader()->hide();

  // vertical header
  momentsEstimationsTable_->createHeaderItem(0, 0, tr("Estimate"));
  momentsEstimationsTable_->createHeaderItem(1, 0, tr("First order mean"));
  momentsEstimationsTable_->createHeaderItem(2, 0, tr("Second order mean"));
  momentsEstimationsTable_->createHeaderItem(3, 0, tr("Standard deviation"));
  momentsEstimationsTable_->createHeaderItem(4, 0, tr("Variance"));

  // horizontal header
  momentsEstimationsTable_->createHeaderItem(0, 1, tr("Value"));

  momentsVbox->addWidget(momentsEstimationsTable_);
  momentsVbox->addStretch();
  tabLayout->addWidget(momentsGroupBox);
  tabLayout->addStretch();

  updateEstimatesTable(0);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void TaylorExpansionsMomentsResultWindow::updateEstimatesTable(int indexOutput)
{
  // momentsEstimationsTable_
  // first order mean
  QTableWidgetItem * item = new QTableWidgetItem(QString::number(result_.getMeanFirstOrder()[indexOutput]));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  momentsEstimationsTable_->setItem(1, 1, item);

  // second order mean
  item = new QTableWidgetItem(QString::number(result_.getMeanSecondOrder()[indexOutput]));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  momentsEstimationsTable_->setItem(2, 1, item);

  // standard deviation
  item = new QTableWidgetItem(QString::number(result_.getStandardDeviation()[indexOutput]));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  momentsEstimationsTable_->setItem(3, 1, item);

  // variance
  item = new QTableWidgetItem(QString::number(result_.getVariance()[indexOutput]));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  momentsEstimationsTable_->setItem(4, 1, item);

  // resize table
  momentsEstimationsTable_->resizeColumnsToContents();
  QSize size = momentsEstimationsTable_->sizeHint();
  int width = 0;
  for (int i=0; i<momentsEstimationsTable_->columnCount(); ++i)
    width += momentsEstimationsTable_->columnWidth(i);
  size.setWidth(width+2);
  int height = 0;
  for (int i=0; i<momentsEstimationsTable_->rowCount(); ++i)
    height += momentsEstimationsTable_->rowHeight(i);
  size.setHeight(height+2);
  momentsEstimationsTable_->setMinimumSize(size);
  momentsEstimationsTable_->setMaximumSize(size);
  momentsEstimationsTable_->updateGeometry();
}
}
