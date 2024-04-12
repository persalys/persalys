//                                               -*- C++ -*-
/**
 *  @brief QRadioButton delegate for checkable QTableview
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

#include "persalys/RadioButtonDelegate.hxx"

namespace PERSALYS
{

void RadioButtonDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
  if (index.row() < firstRowWithButton_)
  {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  // draw a radio button
  QStyleOptionButton optionButton;
  optionButton.rect = option.rect;
  // add margin before the radio button
  const int buttonMargin = 3;
  optionButton.rect.translate(buttonMargin, 0);
  optionButton.text  = index.data(Qt::DisplayRole).toString();

  if (index.data(Qt::CheckStateRole).toBool())
    optionButton.state |= QStyle::State_On;
  else
    optionButton.state |= QStyle::State_Off;

  optionButton.state |= QStyle::State_Enabled;

  QApplication::style()->drawControl(QStyle::CE_RadioButton, &optionButton, painter);
}


}
