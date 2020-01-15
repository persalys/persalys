//                                               -*- C++ -*-
/**
 *  @brief QToolButton to open documentation url
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
#include "persalys/DocumentationToolButton.hxx"

namespace PERSALYS
{

DocumentationToolButton::DocumentationToolButton(const QString& urlLink, const FileTools::docType type, QWidget *parent)
  : QToolButton(parent)
  , urlLink_()
{
  setIcon(QIcon(":/images/documentinfo.png"));
  QString toolTip = tr("Open documentation");
  if (type == FileTools::docOT)
    toolTip = tr("Open the OpenTURNS documentation");
  setToolTip(toolTip);

  if (!urlLink.isEmpty())
  {
    urlLink_ = FileTools::GetDocumentationUrl(urlLink, type);

    connect(this, SIGNAL(clicked()), this, SLOT(openUrl()));
  }
}


void DocumentationToolButton::openUrl()
{
  FileTools::OpenUrl(urlLink_);
}
}
