//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of TaylorExpansionsMomentsAnalysis
 *
 *  Copyright 2015-2016 EDF
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

#include "otgui/TaylorExpansionsMomentsAnalysis.hxx"

#include <QVBoxLayout>

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
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBox_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // -- results --
  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  // first order mean
  QLabel * label = new QLabel(tr("First order mean"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanFirstOrderLabel_ = new QLabel;
  meanFirstOrderLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  grid->addWidget(meanFirstOrderLabel_, gridRow, 1, Qt::AlignTop);

  // second order mean
  label = new QLabel(tr("Second order mean"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanSecondOrderLabel_ = new QLabel;
  meanSecondOrderLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  grid->addWidget(meanSecondOrderLabel_, gridRow, 1, Qt::AlignTop);

  // standard deviation
  label = new QLabel(tr("Standard deviation"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  stdLabel_ = new QLabel;
  stdLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  grid->addWidget(stdLabel_, gridRow, 1, Qt::AlignTop);

  // variance
  label = new QLabel(tr("Variance"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  varianceLabel_ = new QLabel;
  varianceLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  grid->addWidget(varianceLabel_, gridRow, 1);

  grid->setRowStretch(++gridRow, 1);
  grid->setColumnStretch(1, 1);
  tabLayout->addLayout(grid);

  updateLabelsText(0);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void TaylorExpansionsMomentsResultWindow::updateLabelsText(int indexOutput)
{
  // first order mean
  meanFirstOrderLabel_->setText(QString::number(result_.getMeanFirstOrder()[indexOutput]));

  // second order mean
  meanSecondOrderLabel_->setText(QString::number(result_.getMeanSecondOrder()[indexOutput]));

  // standard deviation
  stdLabel_->setText(QString::number(result_.getStandardDeviation()[indexOutput]));

  // variance
  varianceLabel_->setText(QString::number(result_.getVariance()[indexOutput]));
}
}