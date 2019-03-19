//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
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
#include "otgui/StudyTreeView.hxx"

#include "otgui/LineEditWithQValidatorDelegate.hxx"

#include <QMenu>
#include <QHeaderView>
#include <QPainter>
#include <QApplication>

using namespace OT;

namespace OTGUI
{

// define QStyledItemDelegate
class TreeItemDelegate : public LineEditWithQValidatorDelegate
{
public:
  TreeItemDelegate(QObject* parent)
    : LineEditWithQValidatorDelegate(QString("[a-zA-ZáàâäãåçéèêëíìîïñóòôöõúùûüýÿæœÁÀÂÄÃÅÇÉÈÊËÍÌÎÏÑÓÒÔÖÕÚÙÛÜÝŸÆŒ_][a-zA-Z_0-9áàâäãåçéèêëíìîïñóòôöõúùûüýÿæœÁÀÂÄÃÅÇÉÈÊËÍÌÎÏÑÓÒÔÖÕÚÙÛÜÝŸÆŒ]*"), parent)
  {
  }


  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const
  {
    painter->save();

    QStyleOptionViewItem optionButton = option;
    initStyleOption(&optionButton, index);

    // use optionButton.widget to keep information from style sheet
    if (optionButton.widget)
      optionButton.widget->style()->drawControl(QStyle::CE_ItemViewItem, &optionButton, painter, optionButton.widget);
    else
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &optionButton, painter);

    // draw a line at the bottom of items
    if (index.data(Qt::UserRole).toString() == "Study")
    {
      QLineF aLine(optionButton.rect.bottomLeft(), optionButton.rect.bottomRight());
      QPen pen("#0a5205");
      pen.setWidth(1);
      painter->setPen(pen);
      painter->drawLine(aLine);
    }

    painter->restore();
  }


  QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    if (index.data(Qt::UserRole).toString().contains("Title") ||
        index.data(Qt::UserRole).toString() == "Study" ||
        index.data(Qt::UserRole).toString().contains("ModelDiagram") ||
        index.data(Qt::UserRole).toString() == "DesignOfExperimentDefinitionItem" ||
        index.data(Qt::UserRole).toString() == "LimitState"
       )
    {
      result.setHeight(result.height() * 1.5);
    }
    return result;
  }
};


StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel(this))
  , analysisInProgress_(false)
{
  // set model
  Study::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(studyCreated(StudyItem*)), this, SIGNAL(studyCreated(StudyItem*)));
  connect(treeViewModel_, SIGNAL(studySubItemsAdded(StudyItem*)), this, SLOT(modifyStudySubItemsExpansion(StudyItem*)));

  // forbid the user to define not valid item's name
  // draw a line at the bottom of the StudyItem
  setItemDelegate(new TreeItemDelegate(this));

  // context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

  // connections
  connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(selectedItemChanged(QModelIndex, QModelIndex)));

  setExpandsOnDoubleClick(false);

  // style sheet
  const QString styleSheet = " QTreeView::item:selected { background-color: #a5d3a1;\
                                                          color: doubledarkgray;\
                               }\
                               QTreeView::branch:selected { background-color: #a5d3a1;\
                               }\
                               QTreeView::branch:has-children:!has-siblings:closed,\
                               QTreeView::branch:closed:has-children:has-siblings { border-image: none;\
                                                                                    image: url(:/images/branch-closed.png);\
                               }\
                               QTreeView::branch:open:has-children:!has-siblings,\
                               QTreeView::branch:open:has-children:has-siblings { border-image: none;\
                                                                                  image: url(:/images/branch-opened.png);\
                               }\
                               QTreeView::branch:has-siblings:adjoins-item { border-image: url(:/images/branch-more.png) 0;\
                               }\
                               QTreeView::branch:!has-children:!has-siblings:adjoins-item { border-image: url(:/images/branch-end.png) 0;\
                               }\
                             ";

  setStyleSheet(styleSheet);

  // style sheet for header
  const QString headerStyleSheet = " QHeaderView::section { border: 2px solid #0a5205;\
                                                            border-radius: 1px;\
                                                            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                                              stop: 0 #0a5205, stop: 0.4 #4ea248,\
                                                                                              stop: 0.5 #4ea248, stop: 1.0 #0a5205);\
                                                            color: white;\
                                                            font: bold;\
                                                            padding: 5px 1px;\
                                                            outline: none;\
                                                          }\
                                   ";
  header()->setStyleSheet(headerStyleSheet);
}


Item * StudyTreeView::getCurrentItem() const
{
  return dynamic_cast<Item*>(treeViewModel_->itemFromIndex(selectionModel()->currentIndex()));
}


void StudyTreeView::mousePressEvent(QMouseEvent* event)
{
  // get the clicked item
  const QModelIndex eventIndex = indexAt(event->pos());
  const QStandardItem * currentItem = treeViewModel_->itemFromIndex(eventIndex);

  if (currentItem)
  {
    // if it is a title: no selection possible
    if (currentItem->data(Qt::UserRole).toString().contains("Title"))
    {
      if (event->button() == Qt::LeftButton)
        setExpanded(eventIndex, !isExpanded(eventIndex));
      return;
    }
  }

  QTreeView::mousePressEvent(event);
}


void StudyTreeView::onCustomContextMenu(const QPoint &point)
{
  const QModelIndex index = indexAt(point);

  if (!index.isValid())
    return;

  QStandardItem * currentItem = treeViewModel_->itemFromIndex(index);

  QList<QAction*> actions;

  // get actions defined in the current item
  Item * item = dynamic_cast<Item*>(currentItem);
  if (item)
    actions = item->getActions();

  if (!actions.size())
    return;

  // if the item is editable:
  // add rename action at the first position
  QAction * renameAction = 0;
  if (item->isEditable())
  {
    renameAction = new QAction(tr("Rename"), this);
    actions.insert(0, renameAction);
  }

  // build the context menu
  QMenu contextMenu(this);
  contextMenu.addActions(actions);
  QAction * triggeredAction = contextMenu.exec(viewport()->mapToGlobal(point));

  // if rename action
  if (triggeredAction && triggeredAction == renameAction)
  {
    // start editing the item
    emit edit(index);
    delete renameAction;
  }
}


void StudyTreeView::selectedItemChanged(const QModelIndex& currentIndex, const QModelIndex& previousIndex)
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(currentIndex);
  if (!selectedItem)
    return;

  Item * item = dynamic_cast<Item*>(selectedItem);
  if (!item)
    return;

  if (selectedItem->data(Qt::UserRole).toString().contains("Title") && selectedItem->hasChildren())
  {
    setCurrentIndex(item->getParentStudyItem()->index());
    return;
  }

  item->emitShowWindowRequested();
}


void StudyTreeView::modifyStudySubItemsExpansion(StudyItem* item)
{
  setCurrentIndex(item->index());
  setExpanded(item->index(), true);
  for (int i = 0; i < item->rowCount(); ++i)
  {
    setExpanded(item->child(i)->index(), true);
    for (int j = 0; j < item->child(i)->rowCount(); ++j)
    {
      // collapse models items
      setExpanded(item->child(i)->child(j)->index(), false);
    }
  }
}
}
