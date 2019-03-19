//                                               -*- C++ -*-
/**
 *  @brief QPushButton for the diagrams
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
#ifndef OTGUI_DIAGRAMPUSHBUTTON_HXX
#define OTGUI_DIAGRAMPUSHBUTTON_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QPushButton>
#include <QPaintEvent>

namespace OTGUI
{
class OTGUI_API DiagramPushButton : public QPushButton
{
  Q_OBJECT

public:
  DiagramPushButton(const QString& text = "", QWidget* parent = 0);

  void setErrorMessage(const QString& text);

  virtual void paintEvent(QPaintEvent *);
  virtual void enterEvent(QEvent* event);
  virtual void leaveEvent(QEvent* event);
  virtual void mousePressEvent(QMouseEvent *event);

public slots:
  void setEnabled(bool enabled);
signals:
  void buttonInFocus(QString);
  void buttonOutFocus();
  void messageChanged(QString);
  void enabledChanged(bool);

private:
  QString errorMessage_;
  bool valid_;
};
}
#endif