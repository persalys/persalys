//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to define Python script
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
  , nIndent_(0)
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
    updateIndent();
    return;
  }

  QPlainTextEdit::keyPressEvent(e);

  if (e->key() == Qt::Key_Return)
  {
    int position = textCursor().position();
    if(position>1)
      //Look for char before new line added by QPlainTextEdit::keyPressEvent
      if(toPlainText().at(position-2) == ':')
        nIndent_++;
    emit codeEdited(this);
    QTextCursor cursor(textCursor());
    cursor.setPosition(position);
    setTextCursor(cursor);
    for(uint i=0; i<nIndent_; i++)
      cursor.insertText("    ");
  }

  if(e->key() == Qt::Key_Backspace) {
    QTextCursor cursor(textCursor());
    cursor.select(QTextCursor::LineUnderCursor);
    //if empty line
    if(!cursor.selectedText().contains(QRegularExpression("\\S+"))) {
      //clear it
      cursor.removeSelectedText();
      if(nIndent_>0) nIndent_--;
      // add spaces corresponding to new indent
      for(uint i=0; i<nIndent_; i++)
        cursor.insertText("    ");
    }
    cursor.clearSelection();
  }

  updateIndent();

  // update scrollBar Values
  updateVerticalScrollBarValue();
  updateHorizontalScrollBarValue();
}


void CodeEditor::mousePressEvent(QMouseEvent *e) {
  QPlainTextEdit::mousePressEvent(e);
  if(e->button() == Qt::LeftButton)
    updateIndent();
}


void CodeEditor::updateIndent() {
  QTextCursor cursor(textCursor());
  QString currentLine = cursor.block().text();
  QRegularExpression rex("^((\\ {4})+)");
  QRegularExpressionMatch match = rex.match(currentLine);
  if (match.hasMatch())
    nIndent_ = match.capturedTexts().at(0).size()/4;
  else
    nIndent_ = 0;
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
  int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
#else
  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
#endif
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

void CodeEditor::zoomIn(int scale)
{
  QPlainTextEdit::zoomIn(scale);
  lineNumberArea_->setFont(font());
}

void CodeEditor::zoomOut(int scale)
{
  QPlainTextEdit::zoomOut(scale);
  lineNumberArea_->setFont(font());
}

void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    QColor lineColor(ApplicationColor["lighterColor"]);

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
  if(obj == qobject_cast<CodeEditor*>(obj) && event->type() == QEvent::Wheel ) {
    CodeEditor *editor = qobject_cast<CodeEditor*>(obj);
    QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
    if(wheel->modifiers() == Qt::ControlModifier) {
      if(wheel->angleDelta().y() > 0)
        editor->zoomIn(1);
      else
        editor->zoomOut(1);
    }
  }
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


QWidget *CodeDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const
{
  CodeEditor * textEdit = new CodeEditor(parent);
  connect(textEdit, SIGNAL(codeEdited(QWidget *)), this, SIGNAL(commitData(QWidget *)));
  return textEdit;
}


void CodeDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  CodeEditor * textEdit = dynamic_cast<CodeEditor*>(editor);
  CodeHighlighter * highlighter = new CodeHighlighter(textEdit->document());
  (void) highlighter;
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

// ---------------- CodeHighlighter ---------------------------

typedef QMap<QString, QTextCharFormat> FormatMap;
static const FormatMap STYLES = {
    { "keyword", CodeHighlighter::format("blue") },
    { "operator", CodeHighlighter::format("red") },
    { "brace", CodeHighlighter::format("steelblue", "bold") },
    { "defclass", CodeHighlighter::format("black", "bold") },
    { "string", CodeHighlighter::format("magenta") },
    { "string2", CodeHighlighter::format("darkMagenta") },
    { "comment", CodeHighlighter::format("darkGreen", "italic") },
    { "self", CodeHighlighter::format("black", "italic") },
    { "numbers", CodeHighlighter::format("brown") }
};

static const QStringList keywords = {
    "and", "assert", "break", "class", "continue", "def",
    "del", "elif", "else", "except", "exec", "finally",
    "for", "from", "global", "if", "import", "in",
    "is", "lambda", "not", "or", "pass", "print",
    "raise", "return", "try", "while", "yield",
    "None", "True", "False"
};

static const QStringList operators = {
    "=",
    "==", "!=", "<", "<=", ">", ">=",
    "\\+", "-", "\\*", "/", "//", "\\%", "\\*\\*",
    "\\+=", "-=", "\\*=", "/=", "\\%=",
    "\\^", "\\|", "\\&", "\\~", ">>", "<<"
};

static const QStringList braces = {
    "\\{", "\\}", "\\(", "\\)", "\\[", "\\]"
};

CodeHighlighter::CodeHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
  // Keyword, operator, and brace rules
  for(const QString &keyword : keywords)
    {
      QString pattern = QString("\\b%1\\b").arg(keyword);
      highlightingRules += HighlightingRule(pattern, 0, STYLES["keyword"]);
    }

  for(const QString &pattern: operators)
    highlightingRules += HighlightingRule(pattern, 0, STYLES["operator"]);

  for(const QString &pattern: braces)
    highlightingRules += HighlightingRule(pattern, 0, STYLES["brace"]);

  // All other rules

  // 'self'
  highlightingRules += HighlightingRule("\\bself\\b", 0, STYLES["self"]);

  // Quoted string
  highlightingRules += HighlightingRule("\".*\"", 0, STYLES["string"]);
  highlightingRules += HighlightingRule("\'.*\'", 0, STYLES["string"]);

  // 'def' followed by an identifier
  highlightingRules += HighlightingRule("\\bdef\\b\\s*(\\w+)", 1, STYLES["defclass"]);
  // 'class' followed by an identifier
  highlightingRules += HighlightingRule("\\bclass\\b\\s*(\\w+)", 1, STYLES["defclass"]);

  // From '#' until a newline
  highlightingRules += HighlightingRule("#[^\\n]*", 0, STYLES["comment"]);
}

void CodeHighlighter::highlightBlock(const QString &text)
{
  if(text.isEmpty())
    return;
  QRegularExpressionMatchIterator index;
  // Do other syntax formatting
  for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    while (matchIterator.hasNext()) {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
  setCurrentBlockState(0);
}

// Return a QTextCharFormat with the given attributes.
QTextCharFormat CodeHighlighter::format(const QString &colorName, const QString &style)
{
  QColor color;
  color.setNamedColor(colorName);

  QTextCharFormat format;
  format.setForeground(color);

  if(style.contains("bold"))
    format.setFontWeight(QFont::Bold);
  if(style.contains("italic"))
    format.setFontItalic(true);

  return format;
}

}
