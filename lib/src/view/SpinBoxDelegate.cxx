#include "otgui/SpinBoxDelegate.hxx"

#include <QDoubleSpinBox>

namespace OTGUI {

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}


QWidget* SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
  QDoubleSpinBox * editor = new QDoubleSpinBox(parent);
  editor->setFrame(false);
  editor->setMinimum(-1.0);
  editor->setMaximum(1.0);
  editor->setSingleStep(0.05);

  return editor;
}


void SpinBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QDoubleSpinBox * spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
}


void SpinBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QDoubleSpinBox * spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->interpretText();
  model->setData(index, spinBox->value(), Qt::EditRole);
}


void SpinBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex &) const
{
  editor->setGeometry(option.rect);
}


}