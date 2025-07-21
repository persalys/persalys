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
#ifndef PERSALYS_DIAGRAMPUSHBUTTON_HXX
#define PERSALYS_DIAGRAMPUSHBUTTON_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QPushButton>
#include <QPaintEvent>
#include <QPainter>
#include <iostream>

namespace PERSALYS
{

class DiagramPushButton;

class HoverOverlay: public QWidget
{
    Q_OBJECT
public:
    explicit HoverOverlay(const DiagramPushButton *target);

    inline void setErrorMessage(QString errorMessage)
    {errorMessage_ = errorMessage;}

signals:
    void messageChanged(QString);

protected:
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    const DiagramPushButton * target_;
    QString errorMessage_;
};

class PERSALYS_UTILS_API DiagramPushButton : public QPushButton
{
  Q_OBJECT

public:
  DiagramPushButton(const QString& text = "", QWidget* parent = 0);

  virtual void paintEvent(QPaintEvent *);
  virtual void mousePressEvent(QMouseEvent *event);

  HoverOverlay * createOverlay();
  HoverOverlay * getOverlay() const;

public slots:
  void setEnabled(bool enabled, QString errorMessage = "");
signals:
  void buttonInFocus(QString);
  void buttonOutFocus();
  void enabledChanged(bool);

private:
  HoverOverlay *overlay_ = nullptr;
};

}
#endif