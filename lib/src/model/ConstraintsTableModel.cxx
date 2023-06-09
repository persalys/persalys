//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define constraints
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

#include "persalys/ConstraintsTableModel.hxx"

#include <QDebug>
#include <QHeaderView>
#include <QRegularExpression>

using namespace OT;

namespace PERSALYS
{
  ConstraintsTableModel::ConstraintsTableModel(QObject * parent)
    : QAbstractTableModel(parent)
  {
  }

  void ConstraintsTableModel::updateTable(OptimizationAnalysis& analysis)
  {

    leftParts_.clear();
    operators_.clear();
    rightParts_.clear();
    for(UnsignedInteger i=0; i<analysis.getRawEquations().getSize(); ++i)
    {
      std::regex equation("(.*)([<>=])(.*)");
      std::smatch match;
      // Strip spaces
      const String rawEq = std::regex_replace(analysis.getRawEquations()[i],
                                              std::regex(" "), "");

      std::regex_search(rawEq, match, equation);
      if (match.size() != 4)
        throw InvalidArgumentException(HERE) << "Error in loading equation: " << analysis.getRawEquations()[i];
      const String str1 = match[1];
      const String str2 = match[2];
      const String str3 = match[3];
      leftParts_  << QString::fromStdString(str1);
      operators_  << QString::fromStdString(str2);
      rightParts_ << QString::fromStdString(str3);
    }

    // Look if outputs have changed
    for(UnsignedInteger i=0; i<analysis.getPhysicalModel().getOutputs().getSize(); ++i)
    {
      const String output = analysis.getPhysicalModel().getOutputs()[i].getName();
      if (!vars_.contains(QString::fromStdString(output)))
        vars_ << QString::fromStdString(output);
    }
    updateData();

  }


  int ConstraintsTableModel::columnCount(const QModelIndex&) const
  {
    return 3;
  }


  int ConstraintsTableModel::rowCount(const QModelIndex&) const
  {
    Q_ASSERT((leftParts_.size() == operators_.size()) &&
             (leftParts_.size() == rightParts_.size()));
    return leftParts_.size();
  }


  QVariant ConstraintsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
      switch (section)
      {
      case 0:
        return tr("Variable");
      case 1:
        return tr("Operator");
      case 2:
        return tr("Threshold");
      default:
        return QVariant();
      }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
  }


  QVariant ConstraintsTableModel::data(const QModelIndex & index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      switch (index.column())
      {
      case 0:
        return leftParts_[index.row()];
      case 1:
        return operators_[index.row()];
      case 2:
        return rightParts_[index.row()];
      default:
        return QVariant();
      }
    }
    else if (role == Qt::UserRole + 1 && index.column() == 0)
      return vars_;
    else if (role == Qt::UserRole + 1 && index.column() == 1)
      return QStringList() << QString("=") << QString(">") << QString("<");
    else if (role == Qt::BackgroundRole && (index.column() == 0 || index.column() == 1))
      return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);
    return QVariant();
  }


  bool ConstraintsTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    if (!index.isValid())
      return false;

    if (role == Qt::EditRole)
    {
      switch (index.column())
      {
      case 0:
        leftParts_[index.row()] = value.toString();
        break;
      case 1:
        operators_[index.row()] = value.toString();
        break;
      case 2:
        rightParts_[index.row()] = value.toString();
        break;
      }
      emit dataChanged(index, index);
      return true;
    }
    return false;
  }


  Qt::ItemFlags ConstraintsTableModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags result = QAbstractTableModel::flags(index);
    if (index.column() == 0 || index.column() == 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else
      result |= Qt::ItemIsEditable;
    return result;
  }

  void ConstraintsTableModel::updateData()
  {
    beginResetModel();
    endResetModel();
  }

  void ConstraintsTableModel::addLine()
  {
    leftParts_ << QString(vars_.at(0));
    operators_ << QString(">");
    rightParts_ << QString("0");
    QModelIndex lastIndex = index(rowCount() - 1, 0);
    beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row());
    endInsertRows();
    updateData();
  }


  void ConstraintsTableModel::removeLine(const QModelIndex & index)
  {
    beginRemoveRows(index.parent(), index.row(), index.row());
    removeRows(index.row(), 1, index.parent());
    leftParts_.removeAt(index.row());
    operators_.removeAt(index.row());
    rightParts_.removeAt(index.row());
    endRemoveRows();
    updateData();
  }


  QStringList ConstraintsTableModel::getConstraints() const
  {
    QStringList cstrList;
    for(int i=0; i<leftParts_.size(); ++i) {
      QString variable = leftParts_[i];
      cstrList << variable.replace(QRegularExpression("[^0-9a-zA-Z_]"), "_")
        + operators_[i] + rightParts_[i];
    }
    return cstrList;
  }
}
