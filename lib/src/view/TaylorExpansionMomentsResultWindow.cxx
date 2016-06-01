//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of TaylorExpansionMomentsAnalysis
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
#include "otgui/TaylorExpansionMomentsResultWindow.hxx"
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QStackedLayout>

using namespace OT;

namespace OTGUI {

TaylorExpansionMomentsResultWindow::TaylorExpansionMomentsResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<TaylorExpansionMomentsAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void TaylorExpansionMomentsResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // -- output name --
  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  QComboBox * outputsComboBox = new QComboBox;
  QStringList items;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    items << result_.getOutputNames()[i].c_str();
  outputsComboBox->addItems(items);
  headLayout->addWidget(outputsComboBox);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // -- results --

  // moments estimation
  QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimate"));
  QVBoxLayout * momentsVbox = new QVBoxLayout(momentsGroupBox);
  QStackedLayout * momentsTablesLayout = new QStackedLayout;
  connect(outputsComboBox, SIGNAL(currentIndexChanged(int)), momentsTablesLayout, SLOT(setCurrentIndex(int)));

  // loop on all the outputs
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    ResizableTableViewWithoutScrollBar * momentsEstimationsTable = new ResizableTableViewWithoutScrollBar;
    momentsEstimationsTable->horizontalHeader()->hide();
    momentsEstimationsTable->verticalHeader()->hide();
    CustomStandardItemModel * momentsEstimationsTableModel = new CustomStandardItemModel(5, 2);

    // vertical header
    momentsEstimationsTableModel->setNotEditableHeaderItem(0, 0, tr("Estimate"));
    momentsEstimationsTableModel->setNotEditableHeaderItem(1, 0, tr("First order mean"));
    momentsEstimationsTableModel->setNotEditableHeaderItem(2, 0, tr("Second order mean"));
    momentsEstimationsTableModel->setNotEditableHeaderItem(3, 0, tr("Standard deviation"));
    momentsEstimationsTableModel->setNotEditableHeaderItem(4, 0, tr("Variance"));

    // horizontal header
    momentsEstimationsTableModel->setNotEditableHeaderItem(0, 1, tr("Value"));

    // mean
    momentsEstimationsTableModel->setNotEditableItem(1, 1, result_.getMeanFirstOrder()[i]);
    // second order mean
    momentsEstimationsTableModel->setNotEditableItem(2, 1, result_.getMeanSecondOrder()[i]);
    // standard deviation
    momentsEstimationsTableModel->setNotEditableItem(3, 1, result_.getStandardDeviation()[i]);
    // variance
    momentsEstimationsTableModel->setNotEditableItem(4, 1, result_.getVariance()[i]);

    // resize table
    momentsEstimationsTable->setModel(momentsEstimationsTableModel);
    momentsEstimationsTable->resizeToContents();
    momentsTablesLayout->addWidget(momentsEstimationsTable);
  }
  momentsVbox->addLayout(momentsTablesLayout);
  tabLayout->addWidget(momentsGroupBox);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}
}
