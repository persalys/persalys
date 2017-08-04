//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of ApproximationAnalysis
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
#include "otgui/ApproximationResultWindow.hxx"

#include "otgui/ApproximationResultTabWidget.hxx"
#include "otgui/FORMAnalysis.hxx"

#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>

using namespace OT;

namespace OTGUI {

ApproximationResultWindow::ApproximationResultWindow(AnalysisItem* item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
{
  // FORM result widget
  if (dynamic_cast<const FORMAnalysis*>(item->getAnalysis().getImplementation().get()))
  {
    FORMAnalysis analysis = *dynamic_cast<const FORMAnalysis*>(item->getAnalysis().getImplementation().get());
    result_ = analysis.getResult();
    tabWidget_ = new ApproximationResultTabWidget(result_, analysis, this);
  }
  else
  {
    qDebug() << "In ApproximationResultWindow: the given analysis is not a FORMAnalysis";
    throw std::exception();
  }

  buildInterface();
}


void ApproximationResultWindow::buildInterface()
{
  setWindowTitle(tr("Threshold exceedance results"));

  // get output info
  QString outputName(QString::fromUtf8(result_.getLimitStateVariable().getFunction().getOutputDescription()[0].c_str()));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - output
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  OTguiListWidget * outputsListWidget = new OTguiListWidget;
  outputsListWidget->addItems(QStringList() << outputName);
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  if (tabWidget_)
  {
    mainWidget->addWidget(tabWidget_);
    mainWidget->setStretchFactor(1, 10);
  }
  setWidget(mainWidget);
}
}
