#include "ComboBoxDelegate.hxx"
 
#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
 
#include <iostream>
 
ComboBoxDelegate::ComboBoxDelegate(QObject * parent)
  : QItemDelegate(parent)
{
  items_ = QStringList()<<tr("Deterministic")<<tr("Normal");
}
 
 
QWidget *ComboBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QComboBox* editor = new QComboBox(parent);
  editor->addItems(items_);
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
 
// void ComboBoxDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
// {
//   QStyleOptionViewItemV4 myOption = option;
// 
//   myOption.text = index.model()->data(index, Qt::DisplayRole).toString(); 
// //    std::cout<<"value dans paint "<<myOption.text.toStdString()<<std::endl;
//   QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
// 
//  
// 
// }

