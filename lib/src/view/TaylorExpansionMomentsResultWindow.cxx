//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of TaylorExpansionMomentsAnalysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/TaylorExpansionMomentsResultWindow.hxx"

#include "persalys/TaylorExpansionMomentsAnalysis.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

TaylorExpansionMomentsResultWindow::TaylorExpansionMomentsResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_(dynamic_cast<TaylorExpansionMomentsAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
{
  buildInterface();
}


void TaylorExpansionMomentsResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Taylor expansion"), "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#taylorresult"));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputNames().getSize();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputNames()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // - tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // -- results --

  // moments estimation
  QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimates"));
  QVBoxLayout * momentsVbox = new QVBoxLayout(momentsGroupBox);

  QStackedWidget * momentsTablesWidget = new QStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), momentsTablesWidget, SLOT(setCurrentIndex(int)));

  // loop on all the outputs
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    CopyableTableView * momentsEstimationsTable = new CopyableTableView;
    momentsEstimationsTable->horizontalHeader()->hide();
    momentsEstimationsTable->verticalHeader()->hide();
    CustomStandardItemModel * momentsEstimationsTableModel = new CustomStandardItemModel(5, 2, momentsEstimationsTable);

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
    momentsTablesWidget->addWidget(momentsEstimationsTable);
  }
  momentsVbox->addWidget(momentsTablesWidget);
  tabLayout->addWidget(momentsGroupBox, 0, Qt::AlignTop);

  tabWidget->addTab(tab, tr("Summary"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  //
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);

  widgetLayout->addWidget(mainWidget, 1);
}
}
