//                                               -*- C++ -*-
/**
 *  @brief Base class for all the windows of otgui
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_OTGUISUBWINDOW_HXX
#define OTGUI_OTGUISUBWINDOW_HXX

#include <QStandardItem>
#include <QMdiSubWindow>
#include <QLabel>
#include <QTimeLine>

namespace OTGUI {
class OTguiSubWindow : public QMdiSubWindow
{
  Q_OBJECT

public:
  OTguiSubWindow(QStandardItem * item);

  QStandardItem * getItem()const;

  QString getErrorMessage() const;

public slots:
  void setErrorMessage(QString message);
  void setTemporaryErrorMessage(QString message);
  void reInitErrorMessage(QTimeLine::State);
signals:
  void errorMessageChanged(QString);
  void graphWindowActivated(QWidget*);
  void graphWindowDeactivated(QWidget*);

protected:
  QLabel * errorMessageLabel_;

private:
  QStandardItem * item_;
  QString errorMessage_;
  QList<QTimeLine*> qtimelineList_;
};
}
#endif