//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a wizard for parameters definition
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
#include "otgui/ParametersDefinitionWizard.hxx"

#include <QTableView>
#include <QVBoxLayout>
#include <QDebug>

using namespace OT;

namespace OTGUI
{

ParametersTableModel::ParametersTableModel(const Description& names, const Point& values, const QStringList& labels, QObject* parent)
  : QStandardItemModel(names.getSize(), 2, parent)
  , names_(names)
  , values_(values)
{
  if (names_.getSize() != values_.getSize())
  {
    qDebug() << "Error: In ParametersTableModel: arguments have different dimensions";
    throw std::exception();
  }
  setHorizontalHeaderLabels(labels);
}


Qt::ItemFlags  ParametersTableModel::flags(const QModelIndex& index) const
{
  if (!index.column())
    return Qt::ItemIsEditable | QStandardItemModel::flags(index);
  return QStandardItemModel::flags(index);
}


QVariant ParametersTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    if (index.column() == 0)
      return QString::fromUtf8(names_[index.row()].c_str());
    else if (index.column() == 1)
      return QString::number(values_[index.row()]);
  }
  return QVariant();
}


bool  ParametersTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole && index.column() == 1)
  {
    values_[index.row()] = value.toDouble();
    return true;
  }
  return false;
}


Point  ParametersTableModel::getValues() const
{
  return values_;
}


ParametersDefinitionWizard::ParametersDefinitionWizard(const Description& names, const Point& values, const QStringList& labels, QWidget* parent)
  : QWizard(parent)
  , tableModel_(0)
{
  setWindowTitle(tr("Parameter definition"));
  setButtonText(QWizard::FinishButton, tr("Finish"));
  setButtonText(QWizard::CancelButton, tr("Cancel"));
  setOption(QWizard::NoDefaultButton, true);
  setOption(QWizard::NoBackButtonOnStartPage, true);

  QWizardPage * page = new QWizardPage;
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  QTableView * tableView = new QTableView;
  tableModel_ = new ParametersTableModel(names, values, labels, tableView);
  tableView->setModel(tableModel_);

  pageLayout->addWidget(tableView);
  addPage(page);
}


Point ParametersDefinitionWizard::getValues() const
{
  return tableModel_->getValues();
}
}
