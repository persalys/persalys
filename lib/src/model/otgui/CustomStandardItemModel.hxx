//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel to set not editable items and export data
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_CUSTOMSTANDARDITEMMODEL_HXX
#define OTGUI_CUSTOMSTANDARDITEMMODEL_HXX

#include <QStandardItemModel>

#include "otgui/OTGuiprivate.hxx"

namespace OTGUI {
class OTGUI_API CustomStandardItemModel : public QStandardItemModel
{
public:
  CustomStandardItemModel(int nbRows, int nbColumns, QObject * parent = 0);

  void setNotEditableItem(const int row, const int column, const QString text);
  void setNotEditableItem(const int row, const int column, const double value, const int prec=6);
  void setNotEditableHeaderItem(const int row, const int column, const QString text);
  virtual void exportData(const QString & fileName);
};
}
#endif