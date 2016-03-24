//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
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
#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/ModelEvaluation.hxx"
#include "otgui/DataTableWidget.hxx"

#include <QHBoxLayout>
#include <QTableWidget>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

ModelEvaluationResultWindow::ModelEvaluationResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<ModelEvaluation*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void ModelEvaluationResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QHBoxLayout * tabLayout = new QHBoxLayout(tab);

  // inputs table
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  QTableWidget * inputTable = new DataTableWidget(result_.getInputSample().getDimension(), 2);
  inputTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
#if QT_VERSION >= 0x050000
  inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  inputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    QTableWidgetItem * item = new QTableWidgetItem(QString::fromUtf8(result_.getInputSample().getDescription()[i].c_str()));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    inputTable->setItem(i, 0, item);
    item = new QTableWidgetItem(QString::number(result_.getInputSample()[0][i]));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    inputTable->setItem(i, 1, item);
  }
  inputsLayout->addWidget(inputTable);
  tabLayout->addWidget(inputsBox);

  // outputs table
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  QTableWidget * outputTable = new DataTableWidget(result_.getOutputSample().getDimension(), 2);
  outputTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
#if QT_VERSION >= 0x050000
  outputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  outputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

  for (UnsignedInteger i=0; i<result_.getOutputSample().getDimension(); ++i)
  {
    QTableWidgetItem * item = new QTableWidgetItem(QString::fromUtf8(result_.getOutputSample().getDescription()[i].c_str()));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    outputTable->setItem(i, 0, item);
    item = new QTableWidgetItem(QString::number(result_.getOutputSample()[0][i]));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    outputTable->setItem(i, 1, item);
  }
  outputsLayout->addWidget(outputTable);
  tabLayout->addWidget(outputsBox);

  tabWidget->addTab(tab, tr("Result"));

  setWidget(tabWidget);
}
}
