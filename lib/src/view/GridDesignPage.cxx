//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define deterministic designs of experiments
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/GridDesignPage.hxx"

#include "persalys/EditButtonDelegate.hxx"
#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/QtTools.hxx"
#include <persalys/ComboBoxDelegate.hxx>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollBar>

using namespace OT;

namespace PERSALYS
{

GridDesignPage::GridDesignPage(QWidget* parent)
  : QWizardPage(parent)
  , tableView_(0)
  , tableModel_(0)
  , DOESizeLabel_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void GridDesignPage::buildInterface()
{
  setTitle(tr("Full factorial design"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // DOE size
  QWidget * aWidget = new QWidget;
  QGridLayout * sizeLayout = new QGridLayout(aWidget);
  QLabel * sizeLabel = new QLabel(tr("Size of the design of experiments:") + " ");
  sizeLayout->addWidget(sizeLabel, 0, 0);
  DOESizeLabel_ = new QLabel;
  DOESizeLabel_->setObjectName("DOESizeLabel");
  sizeLayout->addWidget(DOESizeLabel_, 0, 1);
  sizeLayout->setColumnStretch(1, 1);
  sizeLayout->setSizeConstraint(QLayout::SetFixedSize);
  pageLayout->addWidget(aWidget);

  // table
  QGroupBox * groupBox = new QGroupBox(tr("Define a grid"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  tableView_ = new ResizableHeaderlessTableView;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  groupBoxLayout->addWidget(tableView_);
  groupBoxLayout->setSizeConstraint(QLayout::SetMaximumSize);

  pageLayout->addWidget(groupBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
}


void GridDesignPage::initialize(const Analysis& analysis)
{
  // get the analysis
  GridDesignOfExperiment * analysis_ptr = dynamic_cast<GridDesignOfExperiment*>(analysis.getImplementation().get());

  GridDesignOfExperiment doe;
  if (analysis_ptr)
  {
    // if already a GridDesignOfExperiment
    doe = *analysis_ptr;
  }
  else
  {
    // create a new analysis
    PhysicalModel physicalModel = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
    doe = GridDesignOfExperiment(analysis.getName(), physicalModel);
  }

  // fill table
  tableModel_ = new ExperimentTableModel(doe, this);
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));
  connect(tableModel_, SIGNAL(doeSizeChanged(QString)), DOESizeLabel_, SLOT(setText(QString)));
  tableView_->setModel(tableModel_);

  // column 5 : level/delta
  ComboBoxDelegate * comboDelegate = new ComboBoxDelegate(tableView_);
  tableView_->setItemDelegateForColumn(5, comboDelegate);
  tableView_->setSpan(0, 5, 1, 3);

  // column 7: edit button
  EditButtonDelegate * delegate = new EditButtonDelegate(tableView_);
  tableView_->setItemDelegateForColumn(7, delegate);

  // column 2-3-4-6: double spinbox
  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(tableView_);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  tableView_->setItemDelegateForColumn(2, spinBoxDelegate);
  tableView_->setItemDelegateForColumn(3, spinBoxDelegate);
  tableView_->setItemDelegateForColumn(4, spinBoxDelegate);
  tableView_->setItemDelegateForColumn(6, spinBoxDelegate);

  for (int i = 1; i < tableModel_->rowCount(); ++i)
  {
    tableView_->openPersistentEditor(tableModel_->index(i, 5));
    tableView_->openPersistentEditor(tableModel_->index(i, 7));
  }

  // resize table
  tableView_->resizeWithOptimalWidth();
  // if too many variables: no fixed height + use scrollbar
  if (tableView_->model()->rowCount() < RowNumberToScrollTable)
  {
    int x1, y1, x2, y2;
    tableView_->getContentsMargins(&x1, &y1, &x2, &y2);
    tableView_->setFixedHeight(tableView_->verticalHeader()->length() + y1 + y2);
  }
  // set DOE size label
  DOESizeLabel_->setText(QString::number(tableModel_->getDesignOfExperiment().getOriginalInputSample().getSize()));
}


void GridDesignPage::resizeEvent(QResizeEvent* event)
{
  QWizardPage::resizeEvent(event);

  if (event->oldSize().width() > 0 && tableView_ && tableModel_)
  {
    // tableView_->resizeWithOptimalWidth() : fixes the width -> the table is not resizable
    // when resizing the wizard, we want to resize the table too
    tableView_->horizontalHeader()->setStretchLastSection(true);
    tableView_->setFixedWidth(QWIDGETSIZE_MAX); // remove constraints
  }
}


Analysis GridDesignPage::getAnalysis()
{
  Q_ASSERT(tableModel_);
  return tableModel_->getDesignOfExperiment();
}


bool GridDesignPage::validatePage()
{
  if (tableModel_->getInterval().isNumericallyEmpty())
  {
    errorMessageLabel_->setErrorMessage(tr("The lower bounds must be less than the upper bounds"));
    return false;
  }

  return true;
}
}
