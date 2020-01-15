//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer
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
#include "persalys/Item.hxx"

#include "persalys/StudyItem.hxx"
#include "persalys/AnalysisItem.hxx"

namespace PERSALYS
{

Item::Item(const QString& name, const QString& type)
  : QStandardItem(name)
  , analysisInProgress_(false)
  , menuActions_()
{
  setData(type, Qt::UserRole);

  if (type.contains("Title"))
  {
    setEditable(false);
    setSelectable(false);
  }
}


Item::~Item()
{
}


void Item::emitShowWindowRequested()
{
  emit showWindowRequested();
}


void Item::emitRemoveWindowRequested()
{
  emit removeWindowRequested();
}


QList< QAction* > Item::getActions()
{
  return menuActions_;
}


void Item::appendAction(QAction* action)
{
  if (action)
    menuActions_.append(action);
}


void Item::appendSeparator(const QString& text)
{
  QAction * separator = new QAction(this);
  separator->setSeparator(true);
  if (!text.isEmpty())
    separator->setText(text);
  appendAction(separator);
}


void Item::insertAction(int i, QAction* action)
{
  if (action)
    menuActions_.insert(i, action);
}


QVariant Item::data(int role) const
{
  const QString dataType(QStandardItem::data(Qt::UserRole).toString());

  if (role == Qt::FontRole && dataType.contains("Title"))
  {
    QFont font;
    font.setBold(true);
    return font;
  }
  else if (role == Qt::FontRole && dataType.contains("Diagram"))
  {
    QFont font;
    font.setUnderline(true);
    return font;
  }
  return QStandardItem::data(role);
}


void Item::appendRow(Item* item)
{
  item->parentStudyItem_ = this->parentStudyItem_;
  connect(item, SIGNAL(removeRequested(int)), item, SIGNAL(removeWindowRequested()));
  connect(item, SIGNAL(removeRequested(int)), this, SLOT(requestRemoveChild(int)));
  QStandardItem::appendRow(item);
}


void Item::insertRow(int row, Item* item)
{
  item->parentStudyItem_ = this->parentStudyItem_;
  connect(item, SIGNAL(removeRequested(int)), item, SIGNAL(removeWindowRequested()));
  connect(item, SIGNAL(removeRequested(int)), this, SLOT(requestRemoveChild(int)));
  QStandardItem::insertRow(row, item);
}


void Item::removeRow(int itemRow)
{
  QStandardItem::removeRow(itemRow);

  // remove itself if it has no more children
  if (data(Qt::UserRole).toString().contains("Title") && !hasChildren())
  {
    emit removeRequested(row());
  }
}


void Item::requestRemoveChild(int row)
{
  removeRow(row);
}


bool Item::analysisInProgress() const
{
  return analysisInProgress_;
}


Item * Item::getTitleItemNamed(const QString &analysisName)
{
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, getParentTitleType(analysisName), 1, Qt::MatchRecursive);
  if (listIndexes.size() == 1)
  {
    QStandardItem * item = model()->itemFromIndex(listIndexes[0]);
    if (item->parent() == this)
      return static_cast<Item*>(item);
  }

  Item * item = getTitleItem(analysisName);
  if (item)
    appendRow(item);
  return item;
}


void Item::appendAnalysisItem(const Analysis &analysis)
{
  // new item
  AnalysisItem * newItem = new AnalysisItem(analysis);

  // append item
  const QString analysisName = analysis.getImplementation()->getClassName().c_str();
  // search title, parent item of the new item
  if (Item * titleItem = getTitleItemNamed(analysisName))
    titleItem->appendRow(newItem);
  else
    appendRow(newItem);

  // emit signal to StudyTreeView to create a window
  emit windowRequested(newItem);
}
}
