#include "otgui/ComboBoxDelegate.hxx"

#include <QComboBox>
#include <QApplication>
#include <QStandardItemModel>

namespace OTGUI {

ComboBoxDelegate::ComboBoxDelegate(QStringList items, QObject * parent)
  : QItemDelegate(parent)
  , items_(items)
{
}


QWidget *ComboBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QComboBox * editor = new QComboBox(parent);
  QStandardItemModel * model = new QStandardItemModel(1, 1);
  QStandardItem * firstItem = new QStandardItem;
  firstItem->setEnabled(false);
  model->setItem(0, 0, firstItem);
  editor->setModel(model);
  editor->addItems(items_);
  connect(editor, SIGNAL(activated(QString)), this, SLOT(emitCommitData()));
  return editor;
}


void ComboBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
  comboBox->setEnabled(true);
  if (comboBox->currentIndex() == -1)
    comboBox->setEnabled(false);
}


void ComboBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->itemText(comboBox->currentIndex()), Qt::EditRole);
}


void ComboBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  editor->setGeometry(option.rect);
}


void ComboBoxDelegate::emitCommitData()
{
  emit closeEditor(qobject_cast<QWidget *>(sender()));
}
}