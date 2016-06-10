//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define deterministic designs of experiments
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
#include "otgui/DeterministicDesignPage.hxx"

#include "otgui/ExperimentTableModel.hxx"
#include "otgui/HorizontalHeaderViewWithCombobox.hxx"
#include "otgui/ComboBoxDelegate.hxx"

#include <NumericalSample.hxx>

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace OTGUI {

DeterministicDesignPage::DeterministicDesignPage(const DesignOfExperiment & designOfExperiment, QWidget* parent)
  : QWizardPage(parent)
  , designOfExperiment_(designOfExperiment)
  , pageValidity_(true)
{
  designOfExperiment_.updateParameters();
  buildInterface();
}

void DeterministicDesignPage::buildInterface()
{
  setTitle(tr("Deterministic design of experiment"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Define manually"));
  buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, DesignOfExperimentImplementation::FromBoundsAndLevels);
  pageLayout->addWidget(buttonToChooseMethod);

  tableView_ = new ResizableTableViewWithoutScrollBar;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  tableView_->setEnabled(true);
  if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromExperiment)
    tableView_->setEnabled(false);

  ExperimentTableModel * model = new ExperimentTableModel(designOfExperiment_);
  connect(model, SIGNAL(designOfExperimentChanged(DesignOfExperiment &)), this, SLOT(setDesignOfExperiment(DesignOfExperiment &)));
  tableView_->setModel(model);

  QStringList items = QStringList()<<tr("Levels")<<tr("Delta");
  QPair<int, int> cellWithComboBox(0, 5);

  tableView_->horizontalHeader()->hide();
  tableView_->setItemDelegateForColumn(5, new ComboBoxDelegate(items, cellWithComboBox));
  tableView_->openPersistentEditor(model->index(0, 5));

  tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView_->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  tableView_->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  // resize table
  tableView_->resizeToContents();

  pageLayout->addWidget(tableView_);
  pageLayout->addStretch();

  // TODO
//   buttonToChooseMethod = new QRadioButton(tr("Use a stratified design"));
//   methodGroup_->addButton(buttonToChooseMethod, DesignOfExperimentImplementation::FromExperiment);
//   pageLayout->addWidget(buttonToChooseMethod);
// 
//   QButtonGroup * designsGroup_ = new QButtonGroup;
//   designsWidget_ = new QWidget;
//   QVBoxLayout * designsLayout = new QVBoxLayout(designsWidget_);
//   QRadioButton * buttonToChooseDesign = new QRadioButton(tr("Axial"));
//   buttonToChooseDesign->setChecked(true);
//   designsGroup_->addButton(buttonToChooseDesign);
//   designsLayout->addWidget(buttonToChooseDesign);
// 
//   buttonToChooseDesign = new QRadioButton(tr("Factorial"));
//   designsGroup_->addButton(buttonToChooseDesign);
//   designsLayout->addWidget(buttonToChooseDesign);
// 
//   buttonToChooseDesign = new QRadioButton(tr("Composite"));
//   designsGroup_->addButton(buttonToChooseDesign);
//   designsLayout->addWidget(buttonToChooseDesign);
// 
//   designsWidget_->setEnabled(true);
//   if (designOfExperiment_.getTypeDesignOfExperiment() != DesignOfExperimentImplementation::FromExperiment)
//     designsWidget_->setEnabled(false);
//   pageLayout->addWidget(designsWidget_);

  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(setEnabledWidgets(int)));
}


void DeterministicDesignPage::setEnabledWidgets(int method)
{
  switch (methodGroup_->checkedId())
  {
    case DesignOfExperimentImplementation::FromBoundsAndLevels:
    {
      tableView_->setEnabled(true);
//       TODO
//       designsWidget_->setEnabled(false);
//       designOfExperiment_ = DesignOfExperiment(designOfExperiment_.getName(),
//                                                designOfExperiment_.getPhysicalModel(),
//                                                Bounds
//                                               );
//       emit designOfExperimentChanged(designOfExperiment_);
      break;
    }
//     case DesignOfExperimentImplementation::FromExperiment:
//     {
//       tableView_->setEnabled(false);
//       designsWidget_->setEnabled(true);
//       TODO
//       designOfExperiment_ = DesignOfExperiment(designOfExperiment_.getName(),
//                                                designOfExperiment_.getPhysicalModel(),
//                                                Experiment
//                                               );
//       emit designOfExperimentChanged(designOfExperiment_);
//       break;
//     }
    default:
      break;
  }
}


void DeterministicDesignPage::setDesignOfExperiment(DesignOfExperiment & designOfExperiment)
{
  designOfExperiment_ = designOfExperiment;
}


bool DeterministicDesignPage::validatePage()
{
  return pageValidity_;
}
}