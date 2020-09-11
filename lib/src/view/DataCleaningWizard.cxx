//                                               -*- C++ -*-
/**
 *  @brief Cleaning data wizard
 *
 *  Copyright 2015-2020 EDF-Phimeca
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

#include "persalys/DataCleaningWizard.hxx"
#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/SampleTableModel.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/LineEditWithQValidatorDelegate.hxx"
#include "persalys/ResizableHeaderlessTableView.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QButtonGroup>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{
  DataCleaningWizard::DataCleaningWizard(DataCleaningTools* cleaner, QWidget * parent)
    : Wizard(parent)
    , cleaner_(cleaner)
  {
    buildInterface();
    if(exec())
      tableModel_->clean();
  }

  void DataCleaningWizard::keyPressEvent (QKeyEvent * event) {
    if(event->key()== Qt::Key_Return || event->key() == Qt::Key_Enter)
      event->accept();
  }

  void DataCleaningWizard::buildInterface() {
    setWindowTitle(tr("Data Cleaning"));
    buildAnalysisPage();
  }

  void DataCleaningWizard::buildAnalysisPage() {
    // build page
    QWizardPage * page = new QWizardPage(this);
    QGridLayout * pageLayout = new QGridLayout(page);

    // table
    QTableView * tableView = new QTableView;
    tableView->horizontalHeader()->hide();

    tableModel_ = new DataCleaningTableModel(cleaner_, tableView);
    tableView->setModel(tableModel_);
    tableView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    pageLayout->addWidget(tableView,0,0,1,4);
    QButtonGroup * buttonGroup = new QButtonGroup(this);

    QRadioButton * meanButton = new QRadioButton(tr("Mean"));
    buttonGroup->addButton(meanButton, 0);
    pageLayout->addWidget(meanButton,1,0);

    QRadioButton * medianButton = new QRadioButton(tr("Median"));
    buttonGroup->addButton(medianButton, 1);
    pageLayout->addWidget(medianButton,2,0);

    QRadioButton * userButton = new QRadioButton(tr("User value"));
    buttonGroup->addButton(userButton, 2);
    pageLayout->addWidget(userButton,3,0);

    lineEdit_ = new ValueLineEdit;
    lineEdit_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lineEdit_->setEnabled(false);
    pageLayout->addWidget(lineEdit_,3,1);

    QRadioButton * removeButton = new QRadioButton(tr("Remove"));
    removeButton->setChecked(true);
    buttonGroup->addButton(removeButton, 3);
    pageLayout->addWidget(removeButton,4,0);

    connect(userButton, &QRadioButton::toggled, [=](Bool enabled) {
        lineEdit_->setEnabled(enabled);});

    connect(meanButton, &QRadioButton::toggled, [=](Bool enabled) {
        if(enabled) {
          tableModel_->updateReplacingValues(cleaner_->getMean());}});

    connect(medianButton, &QRadioButton::toggled, [=](Bool enabled) {
        if(enabled) {
          tableModel_->updateReplacingValues(cleaner_->getMedian());}});

    connect(removeButton, &QRadioButton::toggled, [=](Bool enabled) {
        if(enabled)
          tableModel_->prepareForRemoval();
        else
          tableModel_->prepareForReplacement();});

    connect(lineEdit_, SIGNAL(editingFinished()), this, SLOT(updateUserValues()));
    connect(userButton, SIGNAL(toggled(bool)), this, SLOT(updateUserValues()));

    addPage(page);
  }

  void DataCleaningWizard::updateUserValues() {
    UnsignedInteger nbVars = cleaner_->getSample().getDimension();
    Point vals(nbVars);
    for(UnsignedInteger i=0; i<nbVars; ++i)
      vals[i] = lineEdit_->text().toDouble();
    tableModel_->updateReplacingValues(vals);
  }

}
