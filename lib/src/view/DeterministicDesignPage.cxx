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

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

#include <NumericalSample.hxx>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

DeterministicDesignPage::DeterministicDesignPage(const DesignOfExperiment & designOfExperiment, QWidget* parent)
  : QWizardPage(parent)
{
  FixedDesignOfExperiment fixedDesignOfExperiment;
  if (designOfExperiment.getImplementation()->getClassName() == "FixedDesignOfExperiment")
    fixedDesignOfExperiment = *dynamic_cast<const FixedDesignOfExperiment*>(&*designOfExperiment.getImplementation());
  else
    fixedDesignOfExperiment = FixedDesignOfExperiment(designOfExperiment.getName(), designOfExperiment.getPhysicalModel());

  model_ = new ExperimentTableModel(fixedDesignOfExperiment);

  buildInterface();
}

void DeterministicDesignPage::buildInterface()
{
  setTitle(tr("Deterministic design of experiment"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * groupBox = new QGroupBox(tr("Define manually"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  ResizableTableViewWithoutScrollBar * tableView = new ResizableTableViewWithoutScrollBar;
  tableView->setEditTriggers(QTableView::AllEditTriggers);
  tableView->setModel(model_);

  QStringList items = QStringList()<<tr("Levels")<<tr("Delta");
  QPair<int, int> cellWithComboBox(0, 5);

  tableView->horizontalHeader()->hide();
  tableView->setItemDelegateForColumn(5, new ComboBoxDelegate(items, cellWithComboBox));
  tableView->openPersistentEditor(model_->index(0, 5));

  tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  // resize table
  tableView->resizeToContents();

  groupBoxLayout->addWidget(tableView);
  pageLayout->addWidget(groupBox);
  pageLayout->addStretch();
}


bool DeterministicDesignPage::validatePage()
{
  DesignOfExperiment design = DesignOfExperiment(model_->getDesignOfExperiment());
  emit designOfExperimentChanged(design);

  return true;
}
}