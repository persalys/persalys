//                                               -*- C++ -*-
/**
 *  @brief File tools
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/FileTools.hxx"

#include <QSettings>
#include <QDir>

namespace OTGUI
{

QString FileTools::GetCurrentDir()
{
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings settings;
  QString result = settings.value("currentDir", QDir::homePath()).toString();
  QDir dir(result);
  if (!dir.exists())
  {
    result = QDir::homePath();
  }
  return result;
}


void FileTools::SetCurrentDir(const QString &fileName)
{
  QFileInfo info(fileName);
  if (info.exists())
  {
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    settings.setValue("currentDir", info.absolutePath());
  }
}
}
