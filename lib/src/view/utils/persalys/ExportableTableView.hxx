//                                               -*- C++ -*-
/**
 *  @brief Exportable QTableView
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_EXPORTABLETABLEVIEW_HXX
#define PERSALYS_EXPORTABLETABLEVIEW_HXX

#include "persalys/CopyableTableView.hxx"
#include "persalys/Item.hxx"

#include <openturns/Sample.hxx>

namespace PERSALYS
{
class PERSALYS_API ExportableTableView : public CopyableTableView
{
  Q_OBJECT

public:
  ExportableTableView(QWidget * parent = 0);

  void setExportableAsImage(const bool exportable);

  static QWidget * GetSampleTableViewWidget(Item* item, const OT::Sample& sample);

public slots:
  void contextMenu(const QPoint & pos);
  void exportData();
  void exportImage();

private:
  QAction * exportAction_;
  QAction * exportableAsImageAction_;
};
}
#endif
