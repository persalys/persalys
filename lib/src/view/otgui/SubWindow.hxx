//                                               -*- C++ -*-
/**
 *  @brief Base class for all the windows of otgui
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_SUBWINDOW_HXX
#define OTGUI_SUBWINDOW_HXX

#include "otgui/Item.hxx"

#include <QLabel>

namespace OTGUI
{

// custom QLAbel for all result windows
class OTGUI_API TitleLabel : public QLabel
{
public:
  TitleLabel(const QString &text = "", QWidget * parent = 0)
    : QLabel(text, parent)
  {
    setFrameStyle(QFrame::StyledPanel);
    setMargin(5);
    setStyleSheet("QLabel { font: bold; background-color: white; }");
  }
};


class OTGUI_API SubWindow : public QWidget
{
  Q_OBJECT

public:
  SubWindow(Item * item, QWidget * parent = 0);

  virtual ~SubWindow();

  Item * getItem() const;

  static bool HaveOpenGL32();

public slots:
  void showRequest();
  void removeRequest();
signals:
  void showWindowRequested(QWidget*);
  void removeWindowRequested(QWidget*);
  void graphWindowActivated(QWidget*);
  void graphWindowDeactivated();

private:
  Item * item_;
};
}
#endif
