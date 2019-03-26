//                                               -*- C++ -*-
/**
 *  @brief QHeaderView with combobox
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/HorizontalHeaderViewWithCombobox.hxx"

#include <QLineEdit>

namespace OTGUI
{

HorizontalHeaderViewWithCombobox::HorizontalHeaderViewWithCombobox(QStringList comboBoxItems,
    QVector<int> columns, QWidget * parent)
  : QHeaderView(Qt::Horizontal, parent)
  , comboBoxItems_(comboBoxItems)
  , columnsWithComboBox_(columns)
{
  connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(handleSectionResized(int)));
  connect(this, SIGNAL(sectionMoved(int, int, int)), this, SLOT(handleSectionMoved(int, int, int)));
  setSectionsMovable(true);
}


void HorizontalHeaderViewWithCombobox::showEvent(QShowEvent * e)
{
  // get default color
  const QString defaultColor = QHeaderView(Qt::Horizontal).palette().color(QPalette::Window).name();

  // create and/or update comboboxes
  for (int i = 0; i < columnsWithComboBox_.size(); ++i)
  {
    int j = columnsWithComboBox_[i];
    if (!boxes_[j])
    {
      QComboBox * box = new QComboBox(this);
      box->addItems(comboBoxItems_);
      box->setCurrentIndex(box->findText(model()->headerData(j, Qt::Horizontal).toString()));
      connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(setHeaderData(int)));
      // set text alignment
      box->setEditable(true);
      box->lineEdit()->setReadOnly(true);
      box->lineEdit()->setAlignment(Qt::AlignCenter);
      // set Background color
      box->lineEdit()->setStyleSheet("QLineEdit {background: " + defaultColor + ";}");
      boxes_[j] = box;
    }
    boxes_[j]->setGeometry(sectionViewportPosition(j), 0, sectionSize(j) - 1, height());
    boxes_[j]->show();
  }
  QHeaderView::showEvent(e);
}


void HorizontalHeaderViewWithCombobox::handleSectionResized(int j)
{
  for (int i = 0; i < columnsWithComboBox_.size(); ++i)
  {
    int logical = logicalIndex(columnsWithComboBox_[i]);
    boxes_[logical]->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 1, height());
  }
}

void HorizontalHeaderViewWithCombobox::handleSectionMoved(int logical2, int oldVisualIndex, int newVisualIndex)
{
  for (int i = qMin(oldVisualIndex, newVisualIndex); i < count(); ++i)
  {
    int logical = logicalIndex(columnsWithComboBox_[i]);
    boxes_[logical]->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 1, height());
  }
}


void HorizontalHeaderViewWithCombobox::setHeaderData(int i)
{
  QComboBox * box = qobject_cast<QComboBox *>(sender());

  QMap<int, QComboBox*>::const_iterator it;
  for (it = boxes_.begin(); it != boxes_.end(); ++it)
    if (it.value() == box)
      break;

  if (it == boxes_.end())
    throw;
  model()->setHeaderData(it.key(), Qt::Horizontal, box->currentText(), Qt::DisplayRole);
}


void HorizontalHeaderViewWithCombobox::scrollContentsBy(int dx, int dy)
{
  QHeaderView::scrollContentsBy(dx, dy);
  if (dx != 0)
    fixComboPositions();
}


void HorizontalHeaderViewWithCombobox::fixComboPositions()
{
  for (int i = 0; i < columnsWithComboBox_.size(); ++i)
  {
    int j = columnsWithComboBox_[i];
    boxes_[j]->setGeometry(sectionViewportPosition(j), 0, sectionSize(j) - 1, height());
  }
}
}
