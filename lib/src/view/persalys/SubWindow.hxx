//                                               -*- C++ -*-
/**
 *  @brief Base class for all the windows of persalys
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_SUBWINDOW_HXX
#define PERSALYS_SUBWINDOW_HXX

#include "persalys/Item.hxx"

#include <QLabel>

namespace PERSALYS
{

// custom QLabel for all result windows
class PERSALYS_API TitleLabel : public QWidget
{
public:
  TitleLabel(const QString &text = "", const QString &docLink = "", QWidget * parent = 0);

  void setText(const QString& text);
  void setDocLink(const QString& link);

private:
  QLabel * label_;
};


class PERSALYS_API SubWindow : public QWidget
{
  Q_OBJECT

public:
  SubWindow(Item * item, QWidget * parent = 0);

  virtual ~SubWindow();

  Item * getItem() const;

  // do we support opengl 3.2 context ?
  static bool SupportsOpenGL_3_2();

  // same + returns vendor/version/renderer strings
  static bool SupportsOpenGL_3_2(QStringList & glInfos);

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
