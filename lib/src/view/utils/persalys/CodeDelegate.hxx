//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to define Python script
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_UTILS_API CodeEditor : public QPlainTextEdit
{
  Q_OBJECT

public:
  CodeEditor(QWidget *parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent * event);
  int lineNumberAreaWidth();
  void zoomIn(int scale);
  void zoomOut(int scale);

signals:
  // signal sent when the code is ready to be parsed
  void codeEdited(QWidget * QWidget);

protected:
  void keyPressEvent(QKeyEvent *e) override;
  void mousePressEvent(QMouseEvent *e) override;
  void focusOutEvent(QFocusEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private  slots:
  void updateVerticalScrollBarValue();
  void updateHorizontalScrollBarValue();
  void updateIndent();

  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect &, int);

private:
  int verticalScrollBarValue_;
  int horizontalScrollBarValue_;
  uint nIndent_;
  QWidget * lineNumberArea_;
};


class PERSALYS_UTILS_API LineNumberArea : public QWidget
{
public:
  LineNumberArea(CodeEditor *editor)
  : QWidget(editor)
  , codeEditor_(editor)
  {
  }

  QSize sizeHint() const override
  {
    return QSize(codeEditor_->lineNumberAreaWidth(), 0);
  }

protected:
  void paintEvent(QPaintEvent *event) override
  {
    codeEditor_->lineNumberAreaPaintEvent(event);
  }

private:
    CodeEditor * codeEditor_;
};

class PERSALYS_UTILS_API CodeDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  CodeDelegate(QObject *parent = nullptr);

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

protected:
  bool eventFilter(QObject *obj, QEvent *ev);
};

class PERSALYS_UTILS_API CodeHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  explicit CodeHighlighter(QTextDocument * parent = 0);
  static QTextCharFormat format(const QString &colorName, const QString &style = QString());

protected:
  void highlightBlock(const QString &text) override;

private:
  struct HighlightingRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
    int matchIndex = 0;
    HighlightingRule() { }
    HighlightingRule(const QRegularExpression &r, int i, const QTextCharFormat &f)
      : pattern(r), format(f), matchIndex(i) { }
    HighlightingRule(const QString &p, int i, const QTextCharFormat &f)
      : pattern(QRegularExpression(p)), format(f), matchIndex(i) { }
  };
  bool matchMultiLine(const QString &text, const HighlightingRule &rule);

  QVector<HighlightingRule> highlightingRules;
};
}
#endif
