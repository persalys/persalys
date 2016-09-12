//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define data model
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
#include "otgui/ImportDataModelPage.hxx"

#include "otgui/ComboBoxDelegate.hxx"

#include <QHeaderView>

using namespace OT;

namespace OTGUI {

ImportDataModelPage::ImportDataModelPage(const DataModel & dataModel, QWidget *parent)
  : ImportDataPage(parent)
  , dataModel_(dataModel)
{
  if (dataModel_.getFileName().size())
    setData(QString::fromUtf8(dataModel_.getFileName().c_str()));
}


void ImportDataModelPage::setTable(NumericalSample & sample)
{
  // set table model
  tableModel_ = new DataModelTableModel(sample, dataModel_);
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), this, SLOT(updateErrorMessage(QString)));
  dataPreviewTableView_->setModel(tableModel_);
  
  // set comboboxes items
  QStringList comboBoxItems = QStringList() << tr("Input") << tr("Output") << tr("Disable");

  dataPreviewTableView_->horizontalHeader()->hide();
  dataPreviewTableView_->setItemDelegateForRow(1, new ComboBoxDelegate(comboBoxItems));

  for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
    dataPreviewTableView_->openPersistentEditor(tableModel_->index(1, i));
}


void ImportDataModelPage::setFileName(const QString & fileName)
{
  dataModel_.setFileName(fileName.toLocal8Bit().data());
}


void ImportDataModelPage::updateErrorMessage(const QString & message)
{
  errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(message).arg("</font>"));
  pageValidity_ = message.isEmpty();
}


bool ImportDataModelPage::validatePage()
{
  if (pageValidity_)
    emit dataModelChanged(tableModel_->getDataModel());
  return pageValidity_;
}
}