//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to insert floats in QTableView
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

#ifndef PERSALYS_FLOATDELEGATE_HXX
#define PERSALYS_FLOATDELEGATE_HXX

#include "persalys/PersalysPrivate.hxx"
#include "persalys/DoubleSpinBox.hxx"

#include <QItemDelegate>

namespace PERSALYS
{
  class PERSALYS_UTILS_API FloatDelegate : public QItemDelegate
  {
    Q_OBJECT
  public:
    FloatDelegate(QObject *parent = nullptr)
      : QItemDelegate(parent)
    {
    }

    QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const override
    {
      DoubleSpinBox * editor = new DoubleSpinBox(parent);
      return editor;
    }


    void setEditorData(QWidget * editor, const QModelIndex & index) const override
    {
      DoubleSpinBox * comboBox = static_cast<DoubleSpinBox*>(editor);
      double value = index.model()->data(index, Qt::EditRole).toDouble();
      comboBox->setValue(value);
    }


    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
    {
      DoubleSpinBox * comboBox = static_cast<DoubleSpinBox*>(editor);
      model->setData(index, QString::number(comboBox->value()), Qt::EditRole);
    }

  };
}
#endif
