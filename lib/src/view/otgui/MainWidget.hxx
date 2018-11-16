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
#ifndef OTGUI_MAINWIDGET_HXX
#define OTGUI_MAINWIDGET_HXX

#include "otgui/StudyTreeView.hxx"
#include "otgui/OTguiActions.hxx"
#include "otgui/SubWindowsStackedWidget.hxx"

#include <QDockWidget>

namespace OTGUI
{
class OTGUI_API MainWidget : public QWidget
{
  Q_OBJECT

public:
  MainWidget(QWidget * parent = 0);
  StudyTreeView * getStudyTree() const;
  SubWindowsStackedWidget * getSubWindowsStackedWidget() const;
  OTguiActions * getActions() const;

public slots:
  void showGraphSettingDockWidget(QWidget*);
  void hideGraphSettingDockWidget(QWidget*);

protected:
  void buildInterface();

private:
  StudyTreeView * studyTree_;
  SubWindowsStackedWidget * subWindowsStackedWidget_;
  QDockWidget * graphSettingDockWidget_;
  OTguiActions * actions_;
};
}
#endif
