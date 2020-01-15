//                                               -*- C++ -*-
/**
 *  @brief QHeaderView with a checkable first column
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
#include "persalys/CheckableHeaderView.hxx"

namespace PERSALYS
{

CheckableHeaderView::CheckableHeaderView(Qt::Orientation orientation, QWidget* parent)
  : QHeaderView(orientation, parent)
  , isChecked_(false)
  , modelSignalBlocked_(false)
{
  setSectionsClickable(true);
}


bool CheckableHeaderView::isChecked() const
{
  return isChecked_;
}


void CheckableHeaderView::setModel(QAbstractItemModel* model)
{
  QHeaderView::setModel(model);
  connect(model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this, SLOT(updateCheckState(Qt::Orientation)));
  updateCheckState(orientation());
  // we need to define a minimum section size
  // to avoid that the text takes place under the indicator when resizing the section 0
  if (orientation() == Qt::Horizontal)
    setMinimumSectionSize(getMinimumSectionSize());
  else
    setMinimumWidth(getMinimumSectionSize());
}


int CheckableHeaderView::getMinimumSectionSize() const
{
  // define the minimum section size
  // to avoid that the text takes place under the indicator when resizing the section 0
  QStyleOptionButton option;
  QString textContent;
  if (model())
    textContent = model()->headerData(0, orientation()).toString();

  const int textWidth = style()->itemTextRect(option.fontMetrics, QRect(), Qt::TextShowMnemonic, true, textContent).size().width();
  const int buttonMargin = 3;
  const int labelSpacing = style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);
  const int indicatorWidth = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option).width();

  // before the text we need the size: (buttonMargin + labelSpacing + indicatorWidth)
  // the text is at the middle of the cell
  // so we also need the size (buttonMargin + labelSpacing + indicatorWidth) after the text
  const int minSectionSize = textWidth + 2 * (buttonMargin + labelSpacing + indicatorWidth);

  // if the sort indicator is shown: add sortIndicatorSize + sortIndicatorMargin
  if (orientation() == Qt::Horizontal)
  {
    const int sortIndicatorSize = style()->pixelMetric(QStyle::PM_HeaderMarkSize);
    const int sortIndicatorMargin = style()->pixelMetric(QStyle::PM_HeaderMargin);
    return isSortIndicatorShown() ? (minSectionSize + sortIndicatorSize + sortIndicatorMargin) : minSectionSize;
  }
  else
    return minSectionSize;
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
      // update model Qt::CheckStateRole
      modelSignalBlocked_ = true;
      if (orientation() == Qt::Horizontal)
      {
        for (int i = 0; i < model()->rowCount(); ++i)
        {
          if ((model()->data(model()->index(i, 0), Qt::CheckStateRole).toInt() != (isChecked_ ? Qt::Checked : Qt::Unchecked)) &&
              model()->flags(model()->index(i, 0)) & Qt::ItemIsEnabled)
          {
            model()->setData(model()->index(i, 0), isChecked_ ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
          }
        }
      }
      else
      {
        for (int i = 0; i < model()->columnCount(); ++i)
        {
          if ((model()->data(model()->index(0, i), Qt::CheckStateRole).toInt() != (isChecked_ ? Qt::Checked : Qt::Unchecked)) &&
              model()->flags(model()->index(0, i)) & Qt::ItemIsEnabled)
          {
            model()->setData(model()->index(0, i), isChecked_ ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
          }
        }
      }
      modelSignalBlocked_ = false;
      return;
    }
  }

  QHeaderView::mousePressEvent(event);
}


void CheckableHeaderView::updateCheckState(const Qt::Orientation orientation)
{
  if (modelSignalBlocked_ || !model())
    return;

  bool checkState = true;
  if (orientation == Qt::Horizontal)
  {
    for (int i = 0; i < model()->rowCount(); ++i)
    {
      if (model()->flags(model()->index(i, 0)) & Qt::ItemIsEnabled)
        checkState = checkState && (model()->data(model()->index(i, 0), Qt::CheckStateRole).toInt() == Qt::Checked);
    }
  }
  else
  {
    for (int i = 0; i < model()->columnCount(); ++i)
    {
      if (model()->flags(model()->index(0, i)) & Qt::ItemIsEnabled)
        checkState = checkState && (model()->data(model()->index(0, i), Qt::CheckStateRole).toInt() == Qt::Checked);
    }
  }
  if (isEnabled() && isChecked_ != checkState)
  {
    isChecked_ = !isChecked_;
    updateSection(0);
  }
}
}
