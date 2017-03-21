//                                               -*- C++ -*-
/**
 *  @brief TableView with no scrollbar
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
#ifndef OTGUI_RESIZABLETABLEVIEWWITHOUTSCROLLBAR_HXX
#define OTGUI_RESIZABLETABLEVIEWWITHOUTSCROLLBAR_HXX

#include "otgui/CopyableTableView.hxx"

namespace OTGUI {
class OTGUI_API ResizableTableViewWithoutScrollBar : public CopyableTableView
{
  Q_OBJECT

public:
  ResizableTableViewWithoutScrollBar(QWidget * parent = 0);
  QSize sizeHint() const;
  QSize minimumSizeHint() const;
  void resizeToContents();
};
}
#endif