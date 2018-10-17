//                                               -*- C++ -*-
/**
 *  @brief QMainWindow, main window of the interface
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
#ifndef OTGUI_MAINWINDOW_HXX
#define OTGUI_MAINWINDOW_HXX

#include "otgui/StudyManager.hxx"

#include <QMainWindow>
#include <QCloseEvent>

#include <PyConsole_Console.h>

namespace OTGUI
{

class CustomDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  CustomDockWidget(QString title, QWidget * parent=0)
  : QDockWidget(title, parent)
  {}

  void setVisible(bool visible) override
  {
    emit customVisibilityChanged(visible);
    QDockWidget::setVisible(visible);
  }
signals:
  void customVisibilityChanged(bool);
};


class OTGUI_API MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

protected:
  void buildInterface();

private slots:
  void executePythonCommand(const QString&);
  virtual void closeEvent(QCloseEvent * event);

private:
  StudyManager * manager_;
  PyConsole_Console * pythonConsole_;
};
}
#endif
