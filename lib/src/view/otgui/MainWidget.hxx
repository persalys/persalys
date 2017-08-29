//                                               -*- C++ -*-
/**
 *  @brief QMainWindow, main window of the interface
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_MAINWIDGET_HXX
#define OTGUI_MAINWIDGET_HXX

#include "otgui/StudyTreeView.hxx"
#include "otgui/OTguiMdiArea.hxx"
#include "otgui/OTguiActions.hxx"

#include <QMainWindow>
#include <QDockWidget>
#include <QCloseEvent>

namespace OTGUI {
class OTGUI_API MainWidget : public QWidget
{
  Q_OBJECT

public:
  MainWidget(QWidget* parent=0);
  StudyTreeView * getStudyTree() const;
  OTguiMdiArea * getMdiArea() const;
  OTguiActions * getActions() const;

public slots:
  void showGraphSettingDockWidget(QWidget*);
  void hideGraphSettingDockWidget(QWidget*);
//  void exitApplication();
//  void importPython();

protected:
  void buildInterface();
  void buildActions();

//private slots:
//  virtual void closeEvent(QCloseEvent * event);

private:
  StudyTreeView * studyTree_;
  OTguiMdiArea * mdiArea_;
  QDockWidget * graphSettingDockWidget_;
  OTguiActions * actions_;
};
}
#endif
