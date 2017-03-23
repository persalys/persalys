//                                               -*- C++ -*-
/**
 *  @brief QRadioButton delegate for checkable QTableview
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
#ifndef OTGUI_RADIOBUTTONDELEGATE_HXX
#define OTGUI_RADIOBUTTONDELEGATE_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QStyledItemDelegate>
#include <QApplication>
#include <QPainter>
#include <QRadioButton>

namespace OTGUI {
class RadioButtonDelegate : public QStyledItemDelegate
{
public:
  RadioButtonDelegate(const int firstRowWithButton, QObject *parent)
    : QStyledItemDelegate(parent)
    , firstRowWithButton_(firstRowWithButton)
  {
  }


  void paint(QPainter* painter, 
             const QStyleOptionViewItem& option, 
             const QModelIndex& index) const
  {
    if (index.row() < firstRowWithButton_)
    {
      QStyledItemDelegate::paint(painter, option, index);
      return;
    }

    QStyleOptionButton optionButton;
    optionButton.rect = option.rect;
    optionButton.text  = index.data(Qt::DisplayRole).toString();

    if (index.data(Qt::CheckStateRole).toBool())
      optionButton.state |= QStyle::State_On;
    else
      optionButton.state |= QStyle::State_Off;

    optionButton.state |= QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_RadioButton, &optionButton, painter);
  }


  virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
  {
    return QRadioButton(index.data(Qt::DisplayRole).toString()).sizeHint();
  }

private:
  int firstRowWithButton_;
};
}
#endif