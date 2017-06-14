//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
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
#ifndef OTGUI_DATAMODELWINDOW_HXX
#define OTGUI_DATAMODELWINDOW_HXX

#include "otgui/DataModelDefinitionItem.hxx"
#include "otgui/DataModelTableModel.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/OTguiSubWindow.hxx"

#include <QLineEdit>

namespace OTGUI {

class OTGUI_API DataModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  DataModelWindow(DataModelDefinitionItem * item);

  virtual ~DataModelWindow();

protected:
  void buildInterface();
  void importSample(const QString& fileName);
  void updateTable(const QString& fileName);

public slots:
  void openFileRequested();
  void refreshTable();
  void updateTableView(const OT::NumericalSample& sample=OT::NumericalSample());

private:
  DataModel * dataModel_;
  QLineEdit * filePathLineEdit_;
  QPalette defaultLineEditPalette_;
  ExportableTableView * dataTableView_;
  DataModelTableModel * dataTableModel_;
};
}
#endif