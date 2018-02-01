//                                               -*- C++ -*-
/**
 *  @brief QPushButton for the diagrams
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/DiagramPushButton.hxx"

#include <QStyleOptionButton>
#include <QPainter>

namespace OTGUI
{

DiagramPushButton::DiagramPushButton(const QString& text, QWidget* parent)
  : QPushButton(text, parent)
  , errorMessage_("")
  , valid_(true)
{
  setAttribute(Qt::WA_TranslucentBackground);

  setStyleSheet("QPushButton          { border: 2px solid #0a5205;\
                                        border-radius: 10px;\
                                        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                          stop: 0 #0a5205, stop: 0.4 #4ea248,\
                                                                          stop: 0.5 #4ea248, stop: 1.0 #0a5205);\
                                        color: white;\
                                        font: bold;\
                                        padding: 5px 5px;\
                                        outline: none;\
                                      }\
                 QPushButton:hover    { background-color: #0a5205 }\
                 QPushButton:pressed  { background-color: #4ca545; }\
                 QPushButton:disabled { background-color: #a5d3a1 }\
                ");
}


void DiagramPushButton::setErrorMessage(const QString& text)
{
  errorMessage_ = text;
}


void DiagramPushButton::paintEvent(QPaintEvent*)
{
  QStyleOptionButton option;
  initStyleOption(&option);

  if (!valid_)
    option.state &= ~QStyle::State_Enabled;

  QPainter painter(this);

  style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}


void DiagramPushButton::enterEvent(QEvent* event)
{
  // emit message to update the QTextEdit of the diagram window
  const QString errorMessage = "<p>" + whatsThis() + QString("<p><font color=red>%1</font>").arg(errorMessage_);
  emit messageChanged(valid_ ? whatsThis() : errorMessage);

  // override cursor
  if (!valid_)
    setCursor(Qt::ForbiddenCursor);
  else
    unsetCursor();
}


void DiagramPushButton::leaveEvent(QEvent* event)
{
  emit messageChanged("");
}


void DiagramPushButton::mousePressEvent(QMouseEvent* event)
{
  if (!valid_)
    return;

  QAbstractButton::mousePressEvent(event);
}


void DiagramPushButton::setEnabled(bool enabled)
{
  valid_ = enabled;
  emit enabledChanged(enabled);
}
}