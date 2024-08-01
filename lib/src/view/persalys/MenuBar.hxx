//                                               -*- C++ -*-
/**
 *  @brief QMenuBar
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_MENUBAR_HXX
#define PERSALYS_MENUBAR_HXX

#include "persalys/Actions.hxx"

#include <QMenuBar>

namespace PERSALYS
{
class PERSALYS_VIEW_API MenuBar : public QMenuBar
{
  Q_OBJECT

public:
  enum { NbMaxRecentFiles = 5 };
  MenuBar(const Actions* actions);
  QAction * pythonConsoleDisplayAction() const;
  QAction * settingsDisplayAction() const;

protected:
  void buildActions(const Actions* actions);

public slots:
  void openRecentFile();
  void updateRecentFilesList(const QString & fileName);
  void updateRecentFilesActionsList();
  void clearRecentFilesActions();
  void openUserManual();
  void openAboutDialog();
  void openSettingsWindow();
  void updateConsoleStatus(const bool visibility);
signals:
  void openStudy(QString);
  void showHidePythonConsole(bool);
  void pythonConsoleVisibilityChanged(bool);

private:
  QMenu * recentFilesMenu_;
  QAction * recentFileActions_[NbMaxRecentFiles];
  QAction * pythonConsoleDisplayAction_;
  QAction * settingsDisplayAction_;
};
}
#endif
