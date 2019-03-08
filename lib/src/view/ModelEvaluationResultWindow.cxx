//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
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
#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/ModelEvaluation.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/StudyTreeViewModel.hxx"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI
{

ModelEvaluationResultWindow::ModelEvaluationResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , designOfExperiment_(dynamic_cast<ModelEvaluation*>(item->getAnalysis().getImplementation().get())->getResult().getDesignOfExperiment())
{
  buildInterface();
}


void ModelEvaluationResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Model evaluation"), "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#modelevalresult"));

  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QHBoxLayout * tabLayout = new QHBoxLayout(tab);

  // inputs table
  const UnsignedInteger inputDimension = designOfExperiment_.getInputSample().getDimension();
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  CopyableTableView * inputTable = new CopyableTableView;
#if QT_VERSION >= 0x050000
  inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  inputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
  CustomStandardItemModel * model = new CustomStandardItemModel(inputDimension, 2, inputTable);
  model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
  inputTable->setModel(model);

  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    model->setNotEditableItem(i, 0, QString::fromUtf8(designOfExperiment_.getInputSample().getDescription()[i].c_str()));
    model->setNotEditableItem(i, 1, designOfExperiment_.getInputSample()(0, i), StudyTreeViewModel::DefaultSignificantDigits);
  }
  inputsLayout->addWidget(inputTable);
  tabLayout->addWidget(inputsBox);

  // outputs table
  const UnsignedInteger nbOutputs = designOfExperiment_.getOutputSample().getDimension();
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  CopyableTableView * outputTable = new CopyableTableView;
#if QT_VERSION >= 0x050000
  outputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  outputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
  model = new CustomStandardItemModel(nbOutputs, 2, outputTable);
  model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
  outputTable->setModel(model);

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    model->setNotEditableItem(i, 0, QString::fromUtf8(designOfExperiment_.getOutputSample().getDescription()[i].c_str()));
    model->setNotEditableItem(i, 1, designOfExperiment_.getOutputSample()(0, i), StudyTreeViewModel::DefaultSignificantDigits);
  }
  outputsLayout->addWidget(outputTable);
  tabLayout->addWidget(outputsBox);

  tabWidget->addTab(tab, tr("Result"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  widgetLayout->addWidget(tabWidget);
}
}
