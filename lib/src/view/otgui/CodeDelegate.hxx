#ifndef OTGUI_CODEDELEGATE_H
#define OTGUI_CODEDELEGATE_H

#include <QItemDelegate>

namespace OTGUI {
class CodeDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  CodeDelegate(QObject * parent = 0);
 
  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
//   void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

public slots:
//   void emitCommitData();

private:
//   QStringList items_;
};
}
#endif