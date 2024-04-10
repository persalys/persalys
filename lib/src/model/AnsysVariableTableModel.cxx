//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to handle variables from ansys project file
 *
 *  Copyright 2015-2024 EDF-Phimeca
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

#include "persalys/AnsysVariableTableModel.hxx"

#include "persalys/QtTools.hxx"

using namespace OT;

namespace PERSALYS
{
  VarInfo::VarInfo()
    : value(-1.0), inputOutput(-1), selected(true)
  {}

  AnsysVariableTableModel::AnsysVariableTableModel(QObject * parent)
    : QAbstractTableModel(parent)
  {}

  int AnsysVariableTableModel::columnCount(const QModelIndex & /*parent*/) const
  {
    return 4;
  }

  int AnsysVariableTableModel::rowCount(const QModelIndex & /* parent */) const
  {
    return varInfos_.count();
  }

  QVariant AnsysVariableTableModel::data(const QModelIndex & index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    if (role == Qt::TextAlignmentRole)
      return Qt::AlignLeft;

    VarInfo info(varInfos_[varInfoKeysSorted_[index.row()]]);

    if (role == Qt::CheckStateRole && index.column() == 0)
    {
      return info.selected ? Qt::Checked : Qt::Unchecked;
    }
    else if (role == Qt::DisplayRole)
    {
      switch(index.column())
      {
      case 0:
        return varInfoKeysSorted_[index.row()];
      case 1:
        return info.inputOutput == 0 ? "Input" : "Output";
      case 2:
        return QString::number(info.value);
      case 3:
        return QString(info.unit.c_str());
      default:
        return QVariant();
      }
    }
    return QVariant();
  }

  bool AnsysVariableTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    if (!index.isValid())
      return false;

    if ((index.column() == 0) && (role == Qt::CheckStateRole))
    {
      varInfos_[varInfoKeysSorted_[index.row()]].selected = (value.toInt() == Qt::Checked);
      QModelIndex topLeft = index;
      QModelIndex bottomRight = index;
      emit dataChanged(topLeft, bottomRight);
      emit headerDataChanged(Qt::Horizontal, 0, 0);
      return true;
    }
    return false;
  }

  Qt::ItemFlags AnsysVariableTableModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags mflags = QAbstractTableModel::flags(index);
    if (index.column() == 0)
    {
      mflags &= ~Qt::ItemIsEditable;
      mflags |= Qt::ItemIsUserCheckable;
    }
    return mflags;
  }

  QVariant AnsysVariableTableModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
      switch (section)
      {
      case 0:
        return tr("Variable");
      case 1:
        return tr("I/O");
      case 2:
        return tr("Value");
      case 3:
        return tr("Unit");
      default:
        return QVariant();
      }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
  }


  VarInfo AnsysVariableTableModel::getVarInfo(const QString & varName) const
  {
    return varInfos_[varName];
  }

  QStringList AnsysVariableTableModel::getInputVariables() const{
    QStringList result;
    foreach (QString varName, varInfoKeysSorted_)
    {
      if (varInfos_[varName].selected && (varInfos_[varName].inputOutput == 0))
        result.append(varName);
    }
    return result;
  }

  QStringList AnsysVariableTableModel::getOutputVariables() const{
    QStringList result;
    foreach (QString varName, varInfoKeysSorted_)
    {
      if (varInfos_[varName].selected && (varInfos_[varName].inputOutput > 0))
        result.append(varName);
    }
    return result;
  }


  QString AnsysVariableTableModel::getAnsysVersion() const
  {
    return ansysVersion_;
  }

  void AnsysVariableTableModel::loadData(AnsysParser* parser)
  {
    varInfos_.clear();
    for(UnsignedInteger i = 0; i < parser->getInputVariables().getSize(); ++i)
    {
      VarInfo varInfo;
      varInfo.value = parser->getInputVariables()[i].getValue();
      varInfo.inputOutput = 0;
      varInfo.text = parser->getInputVariables()[i].getDescription();
      varInfo.unit = parser->getInputVariables()[i].getUnit();
      varInfos_[QString(parser->getInputVariables()[i].getName().c_str())] = varInfo;
    }
    for(UnsignedInteger i = 0; i < parser->getOutputVariables().getSize(); ++i)
    {
      VarInfo varInfo;
      varInfo.value = parser->getOutputVariables()[i].getValue();
      varInfo.inputOutput = 1;
      varInfo.text = parser->getOutputVariables()[i].getDescription();
      varInfo.unit = parser->getOutputVariables()[i].getUnit();
      varInfos_[QString(parser->getOutputVariables()[i].getName().c_str())] = varInfo;
    }
    varInfoKeysSorted_ = QtOT::NaturalSorting(varInfos_.keys());

    emit layoutChanged();
  }
}
