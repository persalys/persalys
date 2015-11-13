#include "otgui/ComboBoxDelegate.hxx"
 
#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
 
namespace OTGUI {

ComboBoxDelegate::ComboBoxDelegate(QObject * parent)
  : QItemDelegate(parent)
{
  items_ = QStringList()<<tr("Deterministic")<<tr("Normal");
}
 
 
QWidget *ComboBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QComboBox* editor = new QComboBox(parent);
  editor->addItems(items_);
  connect(editor, SIGNAL(activated(QString)), this, SLOT(emitCommitData()));
  return editor;
}
 
void ComboBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString());
  comboBox->setCurrentIndex(value);
}
 
void ComboBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->itemText(comboBox->currentIndex()), Qt::DisplayRole);
}
 
void ComboBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  editor->setGeometry(option.rect);
}
 

void ComboBoxDelegate::emitCommitData()
{
  emit commitData(qobject_cast<QWidget *>(sender()));
}
}