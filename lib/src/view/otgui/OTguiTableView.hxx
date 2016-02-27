//                                               -*- C++ -*-
/**
 *  @brief QTableView to display samples
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
#ifndef OTGUI_OTGUITABLEVIEW_HXX
#define OTGUI_OTGUITABLEVIEW_HXX

#include "otgui/OTguiTableModel.hxx"

#include <QTableView>

namespace OTGUI {
class OTguiTableView : public QTableView
{
  Q_OBJECT

public:
  OTguiTableView(QWidget *parent=0);
  OTguiTableView(const OT::NumericalSample & sample, QWidget *parent=0);

public slots:
  void contextMenu(const QPoint & pos);
  void exportData();
};
}
#endif