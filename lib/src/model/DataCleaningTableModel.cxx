//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for cleaning data model sample
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


#include "persalys/DataCleaningTableModel.hxx"

#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

  DataCleaningTableModel::DataCleaningTableModel(DataCleaningTools * cleaner, QObject* parent)
    : QAbstractTableModel(parent)
    , cleaner_(cleaner)
    , names_(cleaner_->getSample().getDescription())
    , values_(cleaner_->getSample().getDimension())
    , selected_(cleaner_->getSample().getDimension())
    , removalRequired_(true) {
    selected_.fill();}

  int DataCleaningTableModel::rowCount(const QModelIndex& /*parent*/) const {
    return 5;
  }

  int DataCleaningTableModel::columnCount(const QModelIndex& /*parent*/) const {
    return names_.getSize();
  }

  Qt::ItemFlags DataCleaningTableModel::flags(const QModelIndex & index) const {
    Qt::ItemFlags result = QAbstractTableModel::flags(index);

    if (index.row() == 0)
      result |= Qt::ItemIsUserCheckable;

    else if (index.row() == 4)
      if(!removalRequired_)
        result |= Qt::ItemIsEditable;

    return result;
  }

  QVariant DataCleaningTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) {
      // text
      if (role == Qt::DisplayRole) {
        switch (section) {
        case 0:
          return tr("Variable");
        case 1:
          return tr("Mean");
        case 2:
          return tr("Median");
        case 3:
          return tr("Number of NaN/Inf (%)");
        case 4:
          return tr("Replacing value");
        default:
          return QVariant();
        }
      }
      else if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  QVariant DataCleaningTableModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
      return QVariant();

    if(index.row() == 0) {
      if (role == Qt::DisplayRole)
        return QString::fromStdString(names_[index.column()]);
      else if (role == Qt::CheckStateRole)
        return selected_.contains(index.column()) ? Qt::Checked : Qt::Unchecked;

    } else if(index.row() == 1) {
      if (role == Qt::DisplayRole)
        return cleaner_->getMean()[index.column()];

    } else if(index.row() == 2) {
      if (role == Qt::DisplayRole)
        return cleaner_->getMedian()[index.column()];

    } else if(index.row() == 3) {
      if (role == Qt::DisplayRole)
        return cleaner_->getNanNumbers()[index.column()]/cleaner_->getSample().getSize()*100.;

    } else if(index.row() == 4) {
      if (role == Qt::DisplayRole || role == Qt::EditRole)
        return QString::number(values_[index.column()]);
      else if (role == Qt::BackgroundRole)
        if(removalRequired_)
          return QColor(Qt::gray);
    }
    return QVariant();
  }

  bool DataCleaningTableModel::setData(const QModelIndex & index,
                                       const QVariant & value, int role) {
    if (!index.isValid())
      return false;

    if (index.row() == 0) {
      if (role == Qt::CheckStateRole) {
         if (value.toInt() == Qt::Unchecked) {
           if (selected_.contains(index.column()))
             for (UnsignedInteger i = 0; i < selected_.getSize(); ++i)
               if ((int)selected_[i] == index.column())
                 selected_.erase(selected_.begin() + i);
         } else
           selected_.add(index.column());
      }
    }
    else if (index.row() == 4) {
      if (role == Qt::EditRole)
        values_[index.column()] = value.toDouble();
    }
    return true;
  }

  void DataCleaningTableModel::updateReplacingValues(const Point vals) {
    for(UnsignedInteger i=0; i<selected_.getSize(); ++i)
      values_[selected_[i]] = vals[selected_[i]];
    updateData();
  }

  void DataCleaningTableModel::updateData() {
    beginResetModel();
    endResetModel();
  }

  void DataCleaningTableModel::prepareForRemoval() {
    removalRequired_=true;
    updateData();
  }

  void DataCleaningTableModel::prepareForReplacement() {
    removalRequired_=false;
    updateData();
  }

  Sample DataCleaningTableModel::clean() {
    std::cout << "Cleaning sample " << cleaner_->getSample().getSize() << std::endl;

    if(removalRequired_) {
      for(UnsignedInteger i=0; i<selected_.getSize(); ++i) {
        std::cout << "Cleaning column (remove) " << selected_[i] << std::endl;
        cleaner_->removeNansByColumn(selected_[i]);}
    }
    else {
      for(UnsignedInteger i=0; i<selected_.getSize(); ++i) {
        std::cout << "Cleaning column (replace) " << selected_[i] << std::endl;
        cleaner_->replaceNansByColumn(selected_[i], values_[selected_[i]]);}
    }
    return cleaner_->getSample();
  }

}
