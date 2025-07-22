//                                               -*- C++ -*-
/**
 *  @brief QPushButton for the diagrams
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
#include "persalys/DiagramPushButton.hxx"

#include "persalys/QtTools.hxx"

#include <QStyleOptionButton>
#include <QPainter>

namespace PERSALYS
{

HoverOverlay::HoverOverlay(const DiagramPushButton *target): 
  QWidget(nullptr), target_(target), errorMessage_()
{
  setAttribute(Qt::WA_NoSystemBackground);
}

#if QT_VERSION_CHECK(6,0,0)
void HoverOverlay::enterEvent(QEnterEvent*)
#else
void HoverOverlay::enterEvent(QEvent*)
#endif
{
  // emit message to update the QTextEdit of the diagram window
  const QString errorMessage = "<p>" + target_->whatsThis() + QString("<p><font color=red>%1</font>").arg(errorMessage_);
  emit messageChanged(target_->isEnabled() ? target_->whatsThis() : errorMessage);

  // override cursor
  if (!target_->isEnabled())
    setCursor(Qt::ForbiddenCursor);
  else
    unsetCursor();
}

void HoverOverlay::leaveEvent(QEvent*)
{
  emit messageChanged("");
}

DiagramPushButton::DiagramPushButton(const QString& text, QWidget* parent)
  : QPushButton(text, parent)
{
  setAttribute(Qt::WA_TranslucentBackground);

  const QString dark(ApplicationColor["darkColor"]);
  const QString light(ApplicationColor["lightColor"]);
  const QString medium(ApplicationColor["mediumColor"]);
  setStyleSheet("QPushButton          { border: 2px solid " + dark + ";\
                                        border-radius: 10px;\
                                        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                          stop: 0 " + dark + ", stop: 0.4 " + medium + ",\
                                                                          stop: 0.5 " + medium + ", stop: 1.0 " + dark + ");\
                                        color: white;\
                                        font: bold;\
                                        padding: 5px 5px;\
                                        outline: none;\
                                      }\
                 QPushButton:hover    { background-color: " + dark + " }\
                 QPushButton:pressed  { background-color: " + dark + " }\
                 QPushButton:disabled { background-color: " + light + " }\
                ");
}


void DiagramPushButton::paintEvent(QPaintEvent*)
{
  QStyleOptionButton option;
  initStyleOption(&option);

  if (!isEnabled())
    option.state &= ~QStyle::State_Enabled;

  QPainter painter(this);

  style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}

void DiagramPushButton::mousePressEvent(QMouseEvent* event)
{
  if (!isEnabled())
    return;

  QAbstractButton::mousePressEvent(event);
}

void DiagramPushButton::setEnabled(bool enabled, QString errorMessage)
{
  QPushButton::setEnabled(enabled);
  overlay_->setErrorMessage(errorMessage);
  emit enabledChanged(enabled);
}

HoverOverlay * DiagramPushButton::createOverlay()
{
  if (!overlay_)
    overlay_ = new HoverOverlay(this);
  
  return overlay_;
}

HoverOverlay * DiagramPushButton::getOverlay() const
{
  return overlay_;
}

}
