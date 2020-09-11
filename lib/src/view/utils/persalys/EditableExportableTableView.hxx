//                                               -*- C++ -*-
/**
 *  @brief Editable Exportable QTableView
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

#ifndef PERSALYS_EDITABLEEXPORTABLETABLEVIEW_HXX
#define PERSALYS_EDITABLEEXPORTABLETABLEVIEW_HXX

#include "persalys/ExportableTableView.hxx"

namespace PERSALYS
{
  class PERSALYS_UTILS_API EditableExportableTableView : public ExportableTableView
  {
    Q_OBJECT

  public:
    EditableExportableTableView(QWidget* parent = 0);

  public slots:
    void contextMenu(const QPoint & pos);
    void addRow();
    void removeRows();

  signals:
    void cleanRequested();

  private:
    QAction * addRowAction_;
    QAction * removeRowAction_;
    QAction * cleanAction_;
  };
}

#endif
