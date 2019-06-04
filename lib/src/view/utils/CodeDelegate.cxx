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
#include "persalys/CodeDelegate.hxx"

#include "persalys/QtTools.hxx"

#include <QPlainTextEdit>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QTextCursor>
#include <QScrollBar>

#include <QTextBlock>
#include <QFontMetrics>

namespace PERSALYS
{

// ----------------------- CodeEditor ----------------------------------

CodeEditor::CodeEditor(QWidget * parent)
  : QPlainTextEdit(parent)
  , verticalScrollBarValue_(0)
  , horizontalScrollBarValue_(0)
  , lineNumberArea_(new LineNumberArea(this))
{
#ifndef _WIN32
  QFont font("Monospace");
#else
  QFont font("Courier");
#endif
  font.setPointSize(9);
  font.setFixedPitch(true);
  setWordWrapMode(QTextOption::NoWrap);
  setFont(font);

  connect(verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(updateVerticalScrollBarValue()));
  connect(horizontalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(updateHorizontalScrollBarValue()));

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}


void CodeEditor::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Tab)
  {
    QTextCursor cursor(textCursor());
    cursor.insertText("    ");
    return;
  }

  QPlainTextEdit::keyPressEvent(e);

  if (e->key() == Qt::Key_Return)
  {
    int position = textCursor().position();
    emit codeEdited(this);
    QTextCursor cursor(textCursor());
    cursor.setPosition(position);
    setTextCursor(cursor);
  }
  // update scrollBar Values
  updateVerticalScrollBarValue();
  updateHorizontalScrollBarValue();
}


void CodeEditor::focusOutEvent(QFocusEvent *event)
{
  QPlainTextEdit::focusOutEvent(event);

  verticalScrollBar()->setValue(verticalScrollBarValue_);
  horizontalScrollBar()->setValue(horizontalScrollBarValue_);
}


void CodeEditor::updateVerticalScrollBarValue()
{
  verticalScrollBarValue_ = verticalScrollBar()->sliderPosition();
}


void CodeEditor::updateHorizontalScrollBarValue()
{
  horizontalScrollBarValue_ = horizontalScrollBar()->sliderPosition();
}


int CodeEditor::lineNumberAreaWidth()
{
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10)
  {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits; // TODO 5.12 : use QFontMetrics::horizontalAdvance instead of width

  return space;
}


void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
  if (dy)
    lineNumberArea_->scroll(0, dy);
  else
    lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}


void CodeEditor::resizeEvent(QResizeEvent *e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}


void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    QColor lineColor(ApplicationColor["lightColor"]);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}


void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
  QPainter painter(lineNumberArea_);
  painter.fillRect(event->rect(), Qt::lightGray);


  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0, top, lineNumberArea_->width(), fontMetrics().height(), Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}



// ---------------- CodeDelegate ---------------------------

bool CodeDelegate::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress)
  {
    QKeyEvent * keyEvent = dynamic_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Tab)
    {
      return false;
    }
  }
  return QItemDelegate::eventFilter(obj, event);
}


CodeDelegate::CodeDelegate(QObject * parent)
  : QItemDelegate(parent)
{
}


QWidget *CodeDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  CodeEditor * textEdit = new CodeEditor(parent);
  connect(textEdit, SIGNAL(codeEdited(QWidget *)), this, SIGNAL(commitData(QWidget *)));
  return textEdit;
}


void CodeDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  CodeEditor * textEdit = dynamic_cast<CodeEditor*>(editor);
  QString currentText(textEdit->toPlainText());
  QString newText(index.model()->data(index, Qt::DisplayRole).toString());
  if (currentText != newText)
    textEdit->setPlainText(newText);
}


void CodeDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QPlainTextEdit * textEdit = dynamic_cast<QPlainTextEdit*>(editor);
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
