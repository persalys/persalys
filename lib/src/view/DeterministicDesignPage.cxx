//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define deterministic designs of experiments
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
#include "otgui/DeterministicDesignPage.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollBar>

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

  model_ = new ExperimentTableModel(fixedDesignOfExperiment, this);
  // DOE size
  DOESizeLabel_ = new QLabel(QString::number(fixedDesignOfExperiment.getInputSample().getSize()));
  connect(model_, SIGNAL(doeSizeChanged(QString)), DOESizeLabel_, SLOT(setText(QString)));
  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  connect(model_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setText(QString)));

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

  const QStringList items = QStringList() << tr("Levels") << tr("Delta");
  const QPair<int, int> cellWithComboBox(0, 5);

  tableView->horizontalHeader()->hide();
  tableView->setItemDelegateForColumn(5, new ComboBoxDelegate(items, cellWithComboBox));
  tableView->openPersistentEditor(model_->index(0, 5));

  groupBoxLayout->addWidget(tableView);

  // resize table
  if (tableView->model()->rowCount() > 15) // if too many variables: no fixed height + use scrollbar
  {
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView->resizeColumnsToContents();
    const int w = tableView->horizontalHeader()->length() + tableView->verticalHeader()->width() + tableView->verticalScrollBar()->sizeHint().width();
    int x1, y1, x2, y2;
    tableView->getContentsMargins(&x1, &y1, &x2, &y2);
    tableView->setFixedWidth(w+x1+x2);
  }
  else
  {
    tableView->resizeToContents();
    groupBoxLayout->addStretch();
  }

  pageLayout->addWidget(groupBox);

  // DOE size
  QHBoxLayout * sizeLayout = new QHBoxLayout;
  QLabel * sizeLabel = new QLabel(tr("Size of the design of experiment:") + " ");
  sizeLayout->addWidget(sizeLabel);
  sizeLayout->addWidget(DOESizeLabel_);
  sizeLayout->addStretch();
  pageLayout->addLayout(sizeLayout);

  // error message
  pageLayout->addWidget(errorMessageLabel_);
}


bool DeterministicDesignPage::validatePage()
{
  emit designOfExperimentChanged(model_->getDesignOfExperiment());

  return true;
}
}