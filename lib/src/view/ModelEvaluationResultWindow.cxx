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
#include "otgui/CopyableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <QHBoxLayout>
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

  CopyableTableView * inputTable = new CopyableTableView;
#if QT_VERSION >= 0x050000
  inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  inputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
  CustomStandardItemModel * model = new CustomStandardItemModel(result_.getInputSample().getDimension(), 2);
  model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
  inputTable->setModel(model);

  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    model->setNotEditableItem(i, 0, QString::fromUtf8(result_.getInputSample().getDescription()[i].c_str()));
    model->setNotEditableItem(i, 1, result_.getInputSample()[0][i]);
  }
  inputsLayout->addWidget(inputTable);
  tabLayout->addWidget(inputsBox);

  // outputs table
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  CopyableTableView * outputTable = new CopyableTableView;
#if QT_VERSION >= 0x050000
  outputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  outputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
  model = new CustomStandardItemModel(result_.getOutputSample().getDimension(), 2);
  model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
  outputTable->setModel(model);

  for (UnsignedInteger i=0; i<result_.getOutputSample().getDimension(); ++i)
  {
    model->setNotEditableItem(i, 0, QString::fromUtf8(result_.getOutputSample().getDescription()[i].c_str()));
    model->setNotEditableItem(i, 1, result_.getOutputSample()[0][i]);
  }
  outputsLayout->addWidget(outputTable);
  tabLayout->addWidget(outputsBox);

  tabWidget->addTab(tab, tr("Result"));

  setWidget(tabWidget);
}
}
