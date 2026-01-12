//                                               -*- C++ -*-
/**
 *  @brief QPlainTextEdit for Error and Warnings
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/ErrorWidget.hxx"

#include <iostream>
#include <cmath>
#include <QLabel>

namespace PERSALYS
{
ErrorWidget::ErrorWidget(QWidget *parent)
  : QPlainTextEdit(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setReadOnly(true);
  applyStyle(Information, false);
  setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);  
  QWidget::setVisible(false);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ErrorWidget::~ErrorWidget()
{
  for (const auto * child : findChildren<QTimeLine*>())
    child->disconnect(this);
}

void ErrorWidget::forceVisible(bool on)
{
  forcedVisible_ = on;
  if (on)
    setVisible(true);
}

void ErrorWidget::reset()
{
  if (!forcedVisible_)
    setVisible(false);
  clear();
}

void ErrorWidget::setMessage(const QString& message, MessageType type, bool temporary, bool useFrame)
{
  if (message.isEmpty())
  {
    reset();
    return;
  }

  applyStyle(type, useFrame);

  setVisible(true);

  if (message.endsWith('\n'))
    setPlainText(message.chopped(1));
  else
    setPlainText(message);
  
  if (!userFixedHeight_)
    setFixedHeight(computeHeight());

  if (temporary)
  {
    auto * qtimeline = new QTimeLine(8000, this); // 8 seconds
    connect(qtimeline, &QTimeLine::stateChanged, this, &ErrorWidget::reInitErrorMessage);
    qtimeline->start();
    ++timersCounter_;
  }
}

void ErrorWidget::setFramelessErrorMessage(const QString & message)
{
  setMessage(message, Error, false, false);
}

void ErrorWidget::setTemporaryFramelessErrorMessage(const QString & message)
{
  setMessage(message, Error, true, false);
}

void ErrorWidget::setHeight(int height)
{
  userFixedHeight_ = true;
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setFixedHeight(height);
}

void ErrorWidget::unsetHeight()
{
  userFixedHeight_ = false;
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMinimumHeight(0);
  setMaximumHeight(QWIDGETSIZE_MAX);
}

void ErrorWidget::setWordWrap(bool on)
{
  if (on)
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  else
    setWordWrapMode(QTextOption::NoWrap);
}

int ErrorWidget::computeHeight() const
{
  // Compute document height
  QFontMetrics fm{font()};
  int lineHeight = fm.lineSpacing();

  int docLines = document()->lineCount();
  int contentHeight = docLines * lineHeight;

  // Add margins
  int h = contentHeight
          + contentsMargins().top()
          + contentsMargins().bottom()
          + 2 * frameWidth()
          + 8; // extra padding

  return h;
}

void ErrorWidget::usePadding(bool on)
{
  if (on)
  {
    QPlainTextEdit dummy;
    document()->setDocumentMargin(dummy.document()->documentMargin());
    usePadding_ = true;
  }
  else
  {
    document()->setDocumentMargin(0);
    usePadding_ = false;
  }
}

void ErrorWidget::applyStyle(MessageType type, bool useFrame)
{
  QString styleSheet = "QPlainTextEdit { background-color: transparent; font-weight: bold; ";

  QString color = "black";
  if (type == Error)
    color = "red";
  else if (type == Warning)
    color = "orange";
  
  styleSheet += "color: " + color + ";";

  if (useFrame)
    styleSheet += " border: 1px solid " + color + ";";
  
  if (!usePadding_)
    styleSheet += " padding: 0px;";
  
  styleSheet += " }";

  setStyleSheet(styleSheet);
}

void ErrorWidget::resizeEvent(QResizeEvent *event)
{
  QPlainTextEdit::resizeEvent(event);
  if (!userFixedHeight_)
    setFixedHeight(computeHeight());
}

void ErrorWidget::reInitErrorMessage(QTimeLine::State state)
{
  if (state == QTimeLine::NotRunning && timersCounter_--)
  {
    if (!timersCounter_)
      reset();
  }
}

} // namespace PERSALYS
