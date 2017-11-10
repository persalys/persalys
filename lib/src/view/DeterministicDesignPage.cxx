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
#include "otgui/SpinBoxDelegate.hxx"

#include <QGroupBox>
#include <QScrollBar>

using namespace OT;

namespace OTGUI
{

DeterministicDesignPage::DeterministicDesignPage(QWidget* parent)
  : QWizardPage(parent)
  , groupBoxLayout_(0)
  , tableView_(0)
  , tableModel_(0)
  , DOESizeLabel_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void DeterministicDesignPage::buildInterface()
{
  setTitle(tr("Deterministic design of experiments"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * groupBox = new QGroupBox(tr("Define a grid"));
  groupBoxLayout_ = new QVBoxLayout(groupBox);

  tableView_ = new ResizableHeaderlessTableView;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  tableView_->horizontalHeader()->hide();

  pageLayout->addWidget(groupBox);

  // DOE size
  QWidget * aWidget = new QWidget;
  QHBoxLayout * sizeLayout = new QHBoxLayout(aWidget);
  QLabel * sizeLabel = new QLabel(tr("Size of the design of experiments:") + " ");
  sizeLayout->addWidget(sizeLabel);
  DOESizeLabel_ = new QLabel;
  sizeLayout->addWidget(DOESizeLabel_);
  sizeLayout->addStretch();
  pageLayout->addWidget(aWidget, 0, Qt::AlignBottom);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);

  // register field
  registerField("GridDOESize", DOESizeLabel_, "text", SIGNAL(textChanged()));
}


void DeterministicDesignPage::initialize(const Analysis& analysis)
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
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setText(QString)));
  tableView_->setModel(tableModel_);

  const QStringList items = QStringList() << tr("Levels") << tr("Delta");
  const QPair<int, int> cellWithComboBox(0, 5);
  tableView_->setItemDelegateForColumn(5, new ComboBoxDelegate(items, cellWithComboBox));
  tableView_->openPersistentEditor(tableModel_->index(0, 5));

  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate;
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  for (int i = 1; i < tableModel_->rowCount(); ++i)
    tableView_->setItemDelegateForRow(i, spinBoxDelegate);

  // resize table
  tableView_->resizeWithOptimalWidth();
  if (tableView_->model()->rowCount() < 15) // if too many variables: no fixed height + use scrollbar
  {
    tableView_->resizeWithOptimalHeight();
    groupBoxLayout_->addWidget(tableView_, 0, Qt::AlignTop);
  }
  else
  {
    groupBoxLayout_->addWidget(tableView_);
  }

  // set DOE size label
  DOESizeLabel_->setText(QString::number(doe.getOriginalInputSample().getSize()));
  connect(tableModel_, SIGNAL(doeSizeChanged(QString)), DOESizeLabel_, SLOT(setText(QString)));
}


void DeterministicDesignPage::resizeEvent(QResizeEvent* event)
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


Analysis DeterministicDesignPage::getAnalysis()
{
  Q_ASSERT(tableModel_);
  return tableModel_->getDesignOfExperiment();
}
}
