#include "otgui/CodeDelegate.hxx"

#include <QTextEdit>
#include <QApplication>
#include <QPainter>

namespace OTGUI {

CodeDelegate::CodeDelegate(QObject * parent)
  : QItemDelegate(parent)
{
}


QWidget *CodeDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QTextEdit * textEdit = new QTextEdit(parent);
#ifndef _WIN32
  QFont font("Monospace");
#else
  QFont font("Courier");
#endif
  font.setPointSize(9);
  font.setFixedPitch(true);
  textEdit->setFont(font);
  return textEdit;
}


void CodeDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QTextEdit * textEdit = dynamic_cast<QTextEdit*>(editor);
  textEdit->setPlainText(index.model()->data(index, Qt::DisplayRole).toString());
}


void CodeDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QTextEdit * textEdit = dynamic_cast<QTextEdit*>(editor);
  model->setData(index, textEdit->toPlainText(), Qt::EditRole);
}


void CodeDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QVariant value = index.data(Qt::DisplayRole);
  if (value.isValid() && !value.isNull())
  {
    QTextDocument document;
#ifndef _WIN32
  QFont font("Monospace");
#else
  QFont font("Courier");
#endif
  font.setPointSize(9);
  font.setFixedPitch(true);
    document.setDefaultFont(font);
    document.setDocumentMargin(2);
    document.setPlainText(value.toString());
    painter->translate(option.rect.topLeft());
    document.drawContents(painter);
    painter->translate(-option.rect.topLeft());
  }
}

}