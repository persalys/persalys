//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis by Monte Carlo
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
#include "otgui/MonteCarloReliabilityResultWindow.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>

using namespace OT;

namespace OTGUI {

MonteCarloReliabilityResultWindow::MonteCarloReliabilityResultWindow(ReliabilityAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void MonteCarloReliabilityResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputName = new QLabel(result_.getEvent().getDescription()[0].c_str());
  headLayout->addWidget(outputName);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations : ") + QString::number(result_.getOuterSampling()*result_.getBlockSize()) + "\n");
  grid->addWidget(nbSimuLabel, ++gridRow, 0, 1, 2, Qt::AlignTop);

  QLabel * label = new QLabel(tr("Pf"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);

  double pfCILowerBound = std::max(0.0, result_.getProbabilityEstimate() - 0.5 * result_.getConfidenceLength());
  double pfCIUpperBound = std::min(1.0, result_.getProbabilityEstimate() + 0.5 * result_.getConfidenceLength());
  QString labelText = QString::number(result_.getProbabilityEstimate());
  labelText += "\n CI = [" + QString::number(pfCILowerBound) + ", ";
  labelText += QString::number(pfCIUpperBound) + "] at 95%";
  label = new QLabel(labelText);
  grid->addWidget(label, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("CV Pf"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);

  label = new QLabel(QString::number(result_.getCoefficientOfVariation()));
  grid->addWidget(label, gridRow, 1, Qt::AlignTop);

  grid->setRowStretch(++gridRow, 1);
  grid->setColumnStretch(1, 1);
  tabLayout->addLayout(grid);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}
}