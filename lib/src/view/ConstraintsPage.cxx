//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define optimization constraints
 *
 *  Copyright 2015-2023 EDF-Phimeca
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

#include "persalys/ConstraintsPage.hxx"
#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/FloatDelegate.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>

using namespace OT;

namespace PERSALYS
{

  ConstraintsPage::ConstraintsPage(QWidget* parent)
    : QWizardPage(parent)
  {
    setTitle(tr("Constraints definition"));
    QVBoxLayout * pageLayout = new QVBoxLayout(this);
    QGroupBox * groupBox = new QGroupBox(tr("Add and define constraints"));
    QGridLayout * groupBoxLayout = new QGridLayout(groupBox);

    cstrTableView_ = new QTableView;
    cstrTableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    cstrTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    groupBoxLayout->addWidget(cstrTableView_, 0, 0);

    ComboBoxDelegate * delegate0 = new ComboBoxDelegate(cstrTableView_);
    cstrTableView_->setItemDelegateForColumn(0, delegate0);

    ComboBoxDelegate * delegate1 = new ComboBoxDelegate(cstrTableView_);
    cstrTableView_->setItemDelegateForColumn(1, delegate1);

    FloatDelegate * delegate2 = new FloatDelegate(cstrTableView_);
    cstrTableView_->setItemDelegateForColumn(2, delegate2);

    QPushButton * addInputLineButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
    addInputLineButton->setToolTip(tr("Add a constraint"));
    connect(addInputLineButton, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));

    QPushButton * removeInputLineButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
    removeInputLineButton->setToolTip(tr("Remove the selected constraint"));
    connect(removeInputLineButton, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));

    QHBoxLayout * buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(addInputLineButton);
    buttonsLayout->addWidget(removeInputLineButton);
    groupBoxLayout->addLayout(buttonsLayout, 1, 0);

    groupBoxLayout->setRowStretch(1,1);
    pageLayout->addWidget(groupBox);

    errorMessageLabel_ = new TemporaryLabel;
    pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
  }

  bool ConstraintsPage::validatePage()
  {
    emit constraintsDefined();
    return QWizardPage::validatePage();
  }

  void ConstraintsPage::initialize(OptimizationAnalysis& analysis)
  {
    cstrTableModel_ = new ConstraintsTableModel(this);
    cstrTableView_->setModel(cstrTableModel_);
    cstrTableModel_->updateTable(analysis);
    updateView();

    connect(this, SIGNAL(removeInputLine(QModelIndex)), cstrTableModel_, SLOT(removeLine(QModelIndex)));
  }


  void ConstraintsPage::removeInputLine()
  {
    if (cstrTableView_->selectionModel()->hasSelection())
    {
      QModelIndex index = cstrTableView_->selectionModel()->currentIndex();
      emit removeInputLine(index);
    }
    updateView();
  }

  void ConstraintsPage::addInputLine()
  {
    cstrTableModel_->addLine();
    updateView();
  }

  void ConstraintsPage::updateView()
  {
    for(int i=0; i<cstrTableModel_->rowCount(); ++i)
    {
      cstrTableView_->openPersistentEditor(cstrTableModel_->index(i, 0));
      cstrTableView_->openPersistentEditor(cstrTableModel_->index(i, 1));
      cstrTableView_->openPersistentEditor(cstrTableModel_->index(i, 2));
    }
    const int lastRow = cstrTableView_->model()->rowCount() - 1;
    if (lastRow + 1)
      cstrTableView_->selectRow(lastRow);

  }

  void ConstraintsPage::resizeEvent(QResizeEvent *event) {
    cstrTableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    cstrTableView_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    QWizardPage::resizeEvent(event);
}
}
