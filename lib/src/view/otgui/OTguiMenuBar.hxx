//                                               -*- C++ -*-
/**
 *  @brief QMenuBar
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
#ifndef OTGUI_OTGUIMENUBAR_HXX
#define OTGUI_OTGUIMENUBAR_HXX

#include "otgui/OTguiActions.hxx"

#include <QMenuBar>

namespace OTGUI
{
class OTGUI_API OTguiMenuBar : public QMenuBar
{
  Q_OBJECT

public:
  enum { NbMaxRecentFiles = 5 };
  OTguiMenuBar(const OTguiActions* actions);
  QAction * pythonConsoleDisplayAction() const;

protected:
  void buildActions(const OTguiActions* actions);

public slots:
  void openRecentFile();
  void updateRecentFilesList(const QString & fileName);
  void updateRecentFilesActionsList();
  void clearRecentFilesActions();
  void openUserManual();
  void openAboutDialog();
  void updateConsoleStatus(const bool visibility);
signals:
  void openOTStudy(QString);
  void showHidePythonConsole(bool);
  void pythonConsoleVisibilityChanged(bool);

private:
  QMenu * recentFilesMenu_;
  QAction * recentFileActions_[NbMaxRecentFiles];
  QAction * pythonConsoleDisplayAction_;
};
}
#endif
