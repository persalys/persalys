//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define Morris analysis
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
#include "persalys/MorrisPage.hxx"

#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/ScreeningAnalysisWizard.hxx"
#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QGroupBox>

using namespace OT;

namespace PERSALYS
{

// First Morris page
MorrisPage::MorrisPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void MorrisPage::buildInterface()
{
  setTitle(tr("Morris parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // table
  QGroupBox * groupBox = new QGroupBox(tr("Define a grid"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  tableView_ = new QTableView;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  tableView_->horizontalHeader()->setStretchLastSection(true);
  groupBoxLayout->addWidget(tableView_);
  groupBoxLayout->setSizeConstraint(QLayout::SetMaximumSize);
  pageLayout->addWidget(groupBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
}


void MorrisPage::initialize(const Analysis& analysis)
{
  const MorrisAnalysis * analysis_ptr = dynamic_cast<const MorrisAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  // table
  tableModel_ = new MorrisTableModel(*analysis_ptr, tableView_);
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));
  tableView_->setModel(tableModel_);

  // SpinBoxDelegate for editable the cells
  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(tableView_);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  tableView_->setItemDelegateForColumn(2, spinBoxDelegate);
  tableView_->setItemDelegateForColumn(3, spinBoxDelegate);

  // resize to contents
  tableView_->resizeColumnsToContents();

  // if too many variables: no fixed height + use scrollbar
  if (tableModel_->rowCount() < RowNumberToScrollTable)
  {
    // resize table
    const int h = tableView_->verticalHeader()->length() + tableView_->horizontalHeader()->height();
    const QMargins margins(tableView_->contentsMargins());
    tableView_->setFixedHeight(h + margins.top() + margins.bottom());
  }
}


MorrisAnalysis MorrisPage::getAnalysis() const
{
  return tableModel_->getAnalysis();
}


int MorrisPage::nextId() const
{
  return ScreeningAnalysisWizard::Page_Morris2;
}


bool MorrisPage::validatePage()
{
  // check bounds
  if (tableModel_->getAnalysis().getBounds().isEmpty())
  {
    errorMessageLabel_->setErrorMessage(tr("The lower bounds must be less than the upper bounds"));
    return false;
  }
  // check number of variables which vary
  UnsignedInteger counter = 0;
  for (UnsignedInteger i = 0; i < tableModel_->getAnalysis().getBounds().getDimension(); ++i)
  {
    if (!tableModel_->getAnalysis().getBounds().getMarginal(i).isNumericallyEmpty())
      ++counter;
    if (counter == 2)
      break;
  }
  if (counter < 2)
  {
    errorMessageLabel_->setErrorMessage(tr("At least two variables must vary."));
    return false;
  }

  return true;
}


// Second Morris page
MorrisSecondPage::MorrisSecondPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void MorrisSecondPage::buildInterface()
{
  setTitle(tr("Morris parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // parameters
  QGroupBox * paramGroupBox = new QGroupBox(tr("Parameters"));
  QGridLayout * paramGroupBoxLayout = new QGridLayout(paramGroupBox);

  // trajectories nb
  paramGroupBoxLayout->addWidget(new QLabel(tr("Number of trajectories")), 0, 0);

  trajNbSpinbox_ = new UIntSpinBox;
  paramGroupBoxLayout->addWidget(trajNbSpinbox_, 0, 1);
  connect(trajNbSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(updateNbSimuLabel()));

  // level
  paramGroupBoxLayout->addWidget(new QLabel(tr("Level")), 1, 0);
  levelSpinbox_ = new UIntSpinBox;
  paramGroupBoxLayout->addWidget(levelSpinbox_, 1, 1);
  paramGroupBoxLayout->setSizeConstraint(QLayout::SetMaximumSize);

  pageLayout->addWidget(paramGroupBox);

  // advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedParamGroupBoxLayout = new QGridLayout(advancedParamGroupBox);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedParamGroupBoxLayout->addWidget(seedLabel, 0, 0);
  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedParamGroupBoxLayout->addWidget(seedSpinbox_, 0, 1);

  // block size
  QLabel * blockLabel = new QLabel(tr("Block size"));
  advancedParamGroupBoxLayout->addWidget(blockLabel, 1, 0);
  blockSpinbox_ = new QSpinBox;
  blockSpinbox_->setMinimum(1);
  blockSpinbox_->setMaximum(std::numeric_limits<int>::max());
  blockLabel->setBuddy(blockSpinbox_);
  advancedParamGroupBoxLayout->addWidget(blockSpinbox_, 1, 1);

  advancedParamGroupBox->setExpanded(false);
  pageLayout->addWidget(advancedParamGroupBox);

  // number of Simulations
  pageLayout->addStretch();
  QGridLayout * sizeLayout = new QGridLayout;
  QLabel * sizeLabel = new QLabel(tr("Number of simulations:") + " ");
  sizeLayout->addWidget(sizeLabel, 0, 0);
  nbSimuLabel_ = new QLabel;
  sizeLayout->addWidget(nbSimuLabel_, 0, 1);
  sizeLayout->setColumnStretch(1, 1);
  sizeLayout->setSizeConstraint(QLayout::SetFixedSize);
  pageLayout->addLayout(sizeLayout);
}


void MorrisSecondPage::initialize(const Analysis& analysis)
{
  const MorrisAnalysis * analysis_ptr = dynamic_cast<const MorrisAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  nbInputs_ = analysis_ptr->getPhysicalModel().getInputDimension();
  trajNbSpinbox_->setValue(analysis_ptr->getTrajectoriesNumber());
  levelSpinbox_->setValue(analysis_ptr->getLevel());
  seedSpinbox_->setValue(analysis_ptr->getSeed());
  blockSpinbox_->setValue(analysis_ptr->getBlockSize());
}


int MorrisSecondPage::getTrajectoriesNumber() const
{
  return trajNbSpinbox_->value();
}


int MorrisSecondPage::getLevel() const
{
  return levelSpinbox_->value();
}


int MorrisSecondPage::getSeed() const
{
  return seedSpinbox_->value();
}

int MorrisSecondPage::getBlockSize() const
{
  return blockSpinbox_->value();
}

void MorrisSecondPage::updateNbSimuLabel()
{
  nbSimuLabel_->setText(QString::number((UnsignedInteger)trajNbSpinbox_->value() * (nbInputs_ + 1)));
}
}
