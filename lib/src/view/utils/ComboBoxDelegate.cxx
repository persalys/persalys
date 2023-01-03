//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to insert combobox in QTableView
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/ComboBoxDelegate.hxx"

#include <QWheelEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QFontMetrics>
#include <QPainter>

namespace PERSALYS
{

// CustomComboBox with no wheel event if it has no focus
class CustomComboBox : public QComboBox
{
public:
  CustomComboBox(const bool neverWheelEvent, QWidget *parent)
    : QComboBox(parent)
    , neverWheelEvent_(neverWheelEvent)
  {
    setFocusPolicy(Qt::ClickFocus);
  }

  virtual void wheelEvent(QWheelEvent *e)
  {
    // if no focus : no signal currentIndexChanged emitted -> wrong behavior
    // so if no focus : do nothing
    if (hasFocus() && !neverWheelEvent_)
      QComboBox::wheelEvent(e);
  }
  virtual void keyPressEvent(QKeyEvent *e)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
    if (keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up ||
        keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
    {
      e->ignore();
      return;
    }
    QComboBox::keyPressEvent(e);
  }
private:
  bool neverWheelEvent_;
};


// -- ComboBoxWithSeparatorDelegate

ComboBoxWithSeparatorDelegate::ComboBoxWithSeparatorDelegate(QObject *parent)
  : QItemDelegate(parent)
{
}


void ComboBoxWithSeparatorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator"))
  {
    painter->setPen(Qt::gray);
    painter->drawLine(option.rect.left(), option.rect.center().y(), option.rect.right(), option.rect.center().y());
  }
  else if (index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("parent"))
  {
    QStyleOptionViewItem parentOption = option;
    parentOption.state |= QStyle::State_Enabled;
    QItemDelegate::paint(painter, parentOption, index);
  }
  else if (index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("child"))
  {
    QStyleOptionViewItem childOption = option;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int indent = option.fontMetrics.horizontalAdvance(QString(4, QChar(' ')));
#else
    int indent = painter->fontMetrics().width(QString(4, QChar(' ')));
#endif
    childOption.rect.adjust(indent, 0, 0, 0);
    childOption.textElideMode = Qt::ElideNone;
    QItemDelegate::paint(painter, childOption, index);
  }
  else
  {
    QItemDelegate::paint(painter, option, index);
  }
}


QSize ComboBoxWithSeparatorDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QString type = index.data(Qt::AccessibleDescriptionRole).toString();
  if(type == QLatin1String("separator"))
      return QSize(0, 10);
  return QItemDelegate::sizeHint(option, index);
}


// -- ModelForComboBoxWithSeparator

ModelForComboBoxWithSeparator::ModelForComboBoxWithSeparator(const QStringList &allItems,
                                                             const QList<int> separatorIndex,
                                                             const QStringList &separatorText,
                                                             QWidget *parent)
  : QStandardItemModel(parent)
{
  for (int i = 0; i < allItems.size(); ++i)
  {
    QStandardItem * item = new QStandardItem(allItems[i]);
    item->setData("child", Qt::AccessibleDescriptionRole);
    appendRow(item);
  }
  for (int i = 0; i < separatorIndex.size(); ++i)
  {
    QStandardItem * item = new QStandardItem(separatorText[i]);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setData(separatorText[i].isEmpty() ? "separator" : "parent", Qt::AccessibleDescriptionRole);
    QFont font = item->font();
    font.setBold(true);
    font.setItalic(true);
    item->setFont(font);
    insertRow(separatorIndex[i], item);
  }
}

// ---------- ComboBoxDelegate ----------

ComboBoxDelegate::ComboBoxDelegate(QObject * parent)
  : QItemDelegate(parent)
  , cell_()
  , noWheelEvent_(false)
{
}


ComboBoxDelegate::ComboBoxDelegate(QPair<int, int> cell, QObject * parent)
  : QItemDelegate(parent)
  , cell_(cell)
  , noWheelEvent_(false)
{
}


void ComboBoxDelegate::setNoWheelEvent(const bool noWheelEvent)
{
  noWheelEvent_ = noWheelEvent;
}


void ComboBoxDelegate::addSeparatorIndex(const int index, const QString &text)
{
  separatorIndex_ << index;
  separatorText_ << text;
}


QWidget *ComboBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
      return QItemDelegate::createEditor(parent, option, index);

  QComboBox * editor = new CustomComboBox(noWheelEvent_, parent);
  editor->setItemDelegate(new ComboBoxWithSeparatorDelegate(editor));
  const QStringList items(index.model()->data(index, Qt::UserRole + 1).toStringList());
  editor->addItems(items);
  editor->setEnabled(items.size() > 0);
  // set text alignment
  editor->setEditable(true);
  editor->lineEdit()->setAlignment(Qt::AlignCenter);
  editor->lineEdit()->setReadOnly(true);
  // set Background color
  const QString defaultColor = index.model()->data(index, Qt::BackgroundRole).value<QColor>().name();
  editor->lineEdit()->setStyleSheet("QLineEdit {background: " + defaultColor + ";}");

  connect(editor, SIGNAL(activated(QString)), this, SLOT(emitCommitData()));
  return editor;
}


void ComboBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
      return QItemDelegate::setEditorData(editor, index);

  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  const QStringList items(index.model()->data(index, Qt::UserRole + 1).toStringList());
  comboBox->clear();

  // if has separator : use QStandardItemModel
  if (separatorIndex_.size())
    comboBox->setModel(new ModelForComboBoxWithSeparator(items, separatorIndex_, separatorText_, comboBox));
  // else : use QStringList
  else
    comboBox->addItems(items);

  comboBox->setEnabled(items.size() > 0);
  comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
  comboBox->setEnabled(comboBox->currentIndex() != -1);
}


void ComboBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
      return QItemDelegate::setModelData(editor, model, index);

  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->itemText(comboBox->currentIndex()), Qt::EditRole);
}


void ComboBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & /*index*/) const
{
  editor->setGeometry(option.rect);
}


void ComboBoxDelegate::emitCommitData()
{
  emit closeEditor(qobject_cast<QWidget *>(sender()));
}
}
