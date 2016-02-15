//                                               -*- C++ -*-
/**
 *  @brief QToolBar
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_OTGUITOOLBAR_HXX
#define OTGUI_OTGUITOOLBAR_HXX

#include <QToolBar>

namespace OTGUI {
class OTguiToolBar : public QToolBar
{
  Q_OBJECT

public:
  OTguiToolBar();

protected:
  void buildActions();

public slots:
signals:
  void createNewOTStudy();
  void openOTStudy();
  void loadOTStudy();
  void saveOTStudy();
};
}
#endif