//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer
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
#include "otgui/OTguiItem.hxx"

namespace OTGUI
{

OTguiItem::OTguiItem(const QString& name, const QString& type)
  : QObject()
  , QStandardItem(name)
  , parentOTStudyItem_(0)
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


OTguiItem::~OTguiItem()
{
  parentOTStudyItem_ = 0;
}


void OTguiItem::emitShowWindowRequested()
{
  emit showWindowRequested();
}


void OTguiItem::emitRemoveWindowRequested()
{
  emit removeWindowRequested();
}


OTStudyItem* OTguiItem::getParentOTStudyItem()
{
  return parentOTStudyItem_;
}


QList< QAction* > OTguiItem::getActions()
{
  return menuActions_;
}


void OTguiItem::appendAction(QAction* action)
{
  menuActions_.append(action);
}


void OTguiItem::appendSeparator(const QString& text)
{
  QAction * separator = new QAction(this);
  separator->setSeparator(true);
  if (!text.isEmpty())
    separator->setText(text);
  appendAction(separator);
}


void OTguiItem::insertAction(int i, QAction* action)
{
  menuActions_.insert(i, action);
}


QVariant OTguiItem::data(int role) const
{
  if (QStandardItem::data(Qt::UserRole).toString().contains("Title"))
  {
    if (role == Qt::FontRole)
    {
      QFont font;
      font.setBold(true);
      return font;
    }
    else if (role == Qt::DecorationRole)
    {
      if (QStandardItem::data(Qt::UserRole).toString() == "PhysicalModelsTitle")
        return QIcon(":/images/physicalModel.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "DataModelsTitle")
        return QIcon(":/images/dataModel.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "MetaModelsTitle")
        return QIcon(":/images/metaModel.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "ModelEvaluationTitle")
        return QIcon(":/images/modelEvaluation.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "DesignsOfExperimentTitle")
        return QIcon(":/images/designOfExperiment.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "CentralTendencyTitle")
        return QIcon(":/images/centralTendency.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "SensitivityTitle")
        return QIcon(":/images/sensitivity.png");
      else if (QStandardItem::data(Qt::UserRole).toString() == "ReliabilityTitle")
        return QIcon(":/images/limitstate.png");
    }
  }
  return QStandardItem::data(role);
}


void OTguiItem::appendRow(OTguiItem* item)
{
  item->parentOTStudyItem_ = this->parentOTStudyItem_;
  connect(item, SIGNAL(removeRequested(int)), item, SIGNAL(removeWindowRequested()));
  connect(item, SIGNAL(removeRequested(int)), this, SLOT(requestRemoveChild(int)));
  QStandardItem::appendRow(item);
}


void OTguiItem::insertRow(int row, OTguiItem* item)
{
  item->parentOTStudyItem_ = this->parentOTStudyItem_;
  connect(item, SIGNAL(removeRequested(int)), item, SIGNAL(removeWindowRequested()));
  connect(item, SIGNAL(removeRequested(int)), this, SLOT(requestRemoveChild(int)));
  QStandardItem::insertRow(row, item);
}


void OTguiItem::removeRow(int itemRow)
{
  QStandardItem::removeRow(itemRow);

  // remove itself if it has no more children
  if (data(Qt::UserRole).toString().contains("Title") && !hasChildren())
  {
    emit removeRequested(row());
  }
}


void OTguiItem::requestRemoveChild(int row)
{
  removeRow(row);
}


void OTguiItem::setAnalysisInProgress(bool inProgress)
{
  analysisInProgress_ = inProgress;
}


bool OTguiItem::analysisInProgress() const
{
  return analysisInProgress_;
}


OTguiItem* OTguiItem::getTitleItemNamed(const QString& name, const QString& typeName)
{
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, typeName, 1, Qt::MatchRecursive);
  if (listIndexes.size() == 1)
  {
    QStandardItem * item = model()->itemFromIndex(listIndexes[0]);
    if (item->parent() == this)
      return static_cast<OTguiItem*>(item);
  }

  OTguiItem * item = new OTguiItem(name, typeName);
  appendRow(item);
  return item;
}
}
