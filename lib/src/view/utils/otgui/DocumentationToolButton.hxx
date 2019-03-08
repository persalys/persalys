//                                               -*- C++ -*-
/**
 *  @brief QToolButton to open documentation url
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
#ifndef OTGUI_DOCUMENTATIONTOOLBUTTON_HXX
#define OTGUI_DOCUMENTATIONTOOLBUTTON_HXX

#include "otgui/FileTools.hxx"

#include <QToolButton>
#include <QUrl>

namespace OTGUI
{
class OTGUI_API DocumentationToolButton : public QToolButton
{
  Q_OBJECT

public:
  DocumentationToolButton(const QString& urlLink, const FileTools::docType type = FileTools::docGUI, QWidget *parent = 0);

public slots:
  void openUrl();

private:
  QUrl urlLink_;
};
}
#endif
