//                                               -*- C++ -*-
/**
 *  @brief QComboBox which acts as a selection button
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
#include "otgui/TitledComboBox.hxx"

#include <QPainter>

namespace OTGUI {
  
TitledComboBox::TitledComboBox(const QString & title, QWidget * parent)
  : QComboBox(parent)
  , title_(title)
{
  connect(this, SIGNAL(activated(int)), this, SLOT(activateText(int)));
  // fake item to have the minimum size
  addItem(title_);
  setMinimumSize(sizeHint());
  clear();
}


TitledComboBox::TitledComboBox(const QIcon & icon, const QString & title, QWidget * parent)
  : QComboBox(parent)
  , title_(title)
  , icon_(icon)
{
  connect(this, SIGNAL(activated(int)), this, SLOT(activateText(int)));
  // fake item to have the minimum size
  addItem(icon_, title_);
  setMinimumSize(sizeHint());
  clear();
}


void TitledComboBox::activateText(int index)
{
  QString text(itemText(index));
  emit textActivated(text);
}


void TitledComboBox::paintEvent(QPaintEvent *)
{
  QStyleOptionComboBox option;
  initStyleOption(&option);
  option.currentText = title_;
  option.currentIcon = icon_;
  QPainter painter(this);
  style()->drawComplexControl(QStyle::CC_ComboBox, &option, &painter, this);
  style()->drawControl(QStyle::CE_ComboBoxLabel, &option, &painter, this);
}


void TitledComboBox::wheelEvent(QWheelEvent *)
{
}
}