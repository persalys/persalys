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
#include "otgui/DocumentationToolButton.hxx"

#include <QDesktopServices>
#include <QUrl>

namespace OTGUI
{
const QString DocumentationToolButton::OpenTURNSUrlLink = "http://openturns.github.io/openturns/master/";

DocumentationToolButton::DocumentationToolButton(const QString& urlLink, const QString& toolTip, QWidget *parent)
  : QToolButton(parent)
  , urlLink_(urlLink)
{
  setIcon(QIcon(":/images/documentinfo.png"));
  setToolTip(toolTip);
  connect(this, SIGNAL(clicked()), this, SLOT(openUrl()));
}


void DocumentationToolButton::openUrl()
{
  if (!urlLink_.isEmpty())
    QDesktopServices::openUrl(QUrl(urlLink_));
}
}
