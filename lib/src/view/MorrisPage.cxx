//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define Morris analysis
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/MorrisPage.hxx"

#include "otgui/SpinBoxDelegate.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/ScreeningAnalysisWizard.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QGroupBox>

using namespace OT;

namespace OTGUI
{

// First Morris page
MorrisPage::MorrisPage(QWidget* parent)
  : QWizardPage(parent)
  , tableView_(0)
  , tableModel_(0)
  , errorMessageLabel_(0)
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

  // if too many variables: no fixed height + use scrollbar
  if (tableModel_->rowCount() < 15)
  {
    // resize table
    const int h = tableView_->verticalHeader()->length() + tableView_->horizontalHeader()->height();
    int x1, y1, x2, y2;
    tableView_->getContentsMargins(&x1, &y1, &x2, &y2);
    tableView_->setFixedHeight(h + y1 + y2);
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


// Second Morris page
MorrisSecondPage::MorrisSecondPage(QWidget* parent)
  : QWizardPage(parent)
  , nbInputs_(0)
  , trajNbSpinbox_(0)
  , levelSpinbox_(0)
  , seedSpinbox_(0)
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


void MorrisSecondPage::updateNbSimuLabel()
{
  nbSimuLabel_->setText(QString::number((UnsignedInteger)trajNbSpinbox_->value() * (nbInputs_ + 1)));
}
}
