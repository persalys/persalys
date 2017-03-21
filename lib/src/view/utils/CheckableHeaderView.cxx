//                                               -*- C++ -*-
/**
 *  @brief QHeaderView with a checkable first column
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
#include "otgui/CheckableHeaderView.hxx"

namespace OTGUI {

CheckableHeaderView::CheckableHeaderView(QWidget * parent)
  : QHeaderView(Qt::Horizontal, parent)
  , isChecked_(false)
{
  setToolTip(tr("Select all"));
}


void CheckableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
  painter->save();
  QHeaderView::paintSection(painter, rect, logicalIndex);
  painter->restore();

  if (logicalIndex == 0)
  {
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
    QPoint checkBoxPoint(rect.x() + 3, rect.y() + rect.height() / 2 - checkBoxRect.height() / 2);

    QStyleOptionButton option;
    option.rect = QRect(checkBoxPoint, checkBoxRect.size());
    if (isEnabled())
      option.state |= QStyle::State_Enabled;
    option.state |= isChecked_ ? QStyle::State_On : QStyle::State_Off;

    style()->drawControl(QStyle::CE_CheckBox, &option, painter);
  }
}


void CheckableHeaderView::mousePressEvent(QMouseEvent *event)
{
  if (isEnabled() && logicalIndexAt(event->pos()) == 0)
  {
    isChecked_ = !isChecked_;
    updateSection(0);
    if (model())
      model()->setHeaderData(0, Qt::Horizontal, isChecked_, Qt::CheckStateRole);
    setToolTip(isChecked_? tr("Unselect all"):tr("Select all"));
  }
  else
  {
    QHeaderView::mousePressEvent(event);
  }
}


void CheckableHeaderView::setChecked(bool checked)
{
  if (isEnabled() && isChecked_ != checked)
  {
    isChecked_ = checked;
    updateSection(0);
    setToolTip(isChecked_? tr("Unselect all"):tr("Select all"));
  }
}
}