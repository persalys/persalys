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

CheckableHeaderView::CheckableHeaderView(QWidget* parent)
  : QHeaderView(Qt::Horizontal, parent)
  , isChecked_(false)
{
#if QT_VERSION >= 0x050000
  setSectionsClickable(true);
#else
  setClickable(true);
#endif

  setToolTip(tr("Select all"));
}


void CheckableHeaderView::setModel(QAbstractItemModel* model)
{
  QHeaderView::setModel(model);
  // we need to define a minimum section size
  // to avoid that the text takes place under the indicator when resizing the section 0
  setMinimumSectionSize(getMinimumSectionSize());
}


int CheckableHeaderView::getMinimumSectionSize() const
{
  // define the minimum section size
  // to avoid that the text takes place under the indicator when resizing the section 0
  QStyleOptionButton option;
  QString textContent;
  if (model())
    textContent = model()->headerData(0, Qt::Horizontal).toString();

  const int textWidth = style()->itemTextRect(option.fontMetrics, QRect(), Qt::TextShowMnemonic, true, textContent).size().width();
  const int buttonMargin = 3;
  const int labelSpacing = style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);
  const int indicatorWidth = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option).width();

  // before the text we need the size: (buttonMargin + labelSpacing + indicatorWidth)
  // the text is at the middle of the cell
  // so we also need the size (buttonMargin + labelSpacing + indicatorWidth) after the text
  return textWidth + 2 * (buttonMargin + labelSpacing + indicatorWidth);
}


void CheckableHeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
  painter->save();
  QHeaderView::paintSection(painter, rect, logicalIndex);
  painter->restore();

  if (logicalIndex == 0)
  {
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
    const int buttonMargin = 3;
    QPoint checkBoxPoint(rect.x() + buttonMargin, rect.y() + rect.height() / 2 - checkBoxRect.height() / 2);

    QStyleOptionButton option;
    option.rect = QRect(checkBoxPoint, checkBoxRect.size());
    if (isEnabled())
      option.state |= QStyle::State_Enabled;
    option.state |= isChecked_ ? QStyle::State_On : QStyle::State_Off;

    style()->drawControl(QStyle::CE_CheckBox, &option, painter);
  }
}


void CheckableHeaderView::mousePressEvent(QMouseEvent* event)
{
  if (isEnabled() && logicalIndexAt(event->pos()) == 0 && model())
  {
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
    const int sectionPosX = sectionViewportPosition(0);
    const int buttonMargin = 3;

    if (event->x() <= (sectionPosX + buttonMargin + checkBoxRect.width()) &&
        event->x() >= (sectionPosX + buttonMargin))
    {
      isChecked_ = !isChecked_;
      updateSection(0);
      model()->setHeaderData(0, Qt::Horizontal, isChecked_, Qt::CheckStateRole);

      setToolTip(isChecked_? tr("Unselect all") : tr("Select all"));
      return;
    }
  }

  QHeaderView::mousePressEvent(event);
}


void CheckableHeaderView::setChecked(bool checked)
{
  if (isEnabled() && isChecked_ != checked)
  {
    isChecked_ = checked;
    updateSection(0);
    setToolTip(isChecked_? tr("Unselect all") : tr("Select all"));
  }
}
}