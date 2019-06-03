//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to define Python script
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
#ifndef PERSALYS_CODEDELEGATE_HXX
#define PERSALYS_CODEDELEGATE_HXX

#include <QItemDelegate>
#include <QPlainTextEdit>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{

class PERSALYS_API CodeEditor : public QPlainTextEdit
{
  Q_OBJECT

public:
  CodeEditor(QWidget * parent = 0);

signals:
  // signal sent when the code is ready to be parsed
  void codeEdited(QWidget * QWidget);

protected:
  virtual void keyPressEvent(QKeyEvent *e);
  virtual void focusOutEvent(QFocusEvent *event);
private  slots:
  void updateVerticalScrollBarValue();
  void updateHorizontalScrollBarValue();

private:
  int verticalScrollBarValue_;
  int horizontalScrollBarValue_;
};


class PERSALYS_API CodeDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  CodeDelegate(QObject * parent = 0);

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

protected:
  bool eventFilter(QObject *obj, QEvent *ev);
};
}
#endif
