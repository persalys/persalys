//                                               -*- C++ -*-
/**
 *  @brief QWidget to display Parameters in a table
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_PARAMETERSTABLEVIEW_HXX
#define PERSALYS_PARAMETERSTABLEVIEW_HXX

#include "persalys/CopyableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"

#include <openturns/Exception.hxx>

#include <QHeaderView>
#include <QIdentityProxyModel>

namespace PERSALYS
{

class PERSALYS_UTILS_API LongStringProxy : public QIdentityProxyModel
{
  Q_OBJECT
public:
  LongStringProxy(int column, int lineLength, QObject * parent = nullptr)
    : QIdentityProxyModel(parent)
    , column_(column)
    , lineLength_(lineLength) {}
  QVariant data(const QModelIndex & index, int role) const override;
private:
  int column_;
  int lineLength_;
};

class PERSALYS_UTILS_API ParametersTableView : public CopyableTableView
{
public:
  ParametersTableView(const QStringList names,             // parameters names
                      const QStringList values,            // parameters values
                      const bool showGrid = false,         // show the grid of the table
                      const bool namesHasHeaderType = false, // parameters names display has table header
                      const bool splitLongLines = true,    // split long lines
                      QWidget *parent = nullptr
                     );
};
}
#endif
