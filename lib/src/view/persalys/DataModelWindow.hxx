//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
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
#ifndef PERSALYS_DATAMODELWINDOW_HXX
#define PERSALYS_DATAMODELWINDOW_HXX

#include "persalys/DataModelDefinitionItem.hxx"
#include "persalys/DataModelTableModel.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/SubWindow.hxx"
#include "persalys/ResizableHeaderlessTableView.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QLineEdit>
#include <QResizeEvent>

namespace PERSALYS
{

class PERSALYS_API DataModelWindow : public SubWindow
{
  Q_OBJECT

public:
  DataModelWindow(DataModelDefinitionItem * item, QWidget * parent = 0);

  virtual ~DataModelWindow();

  virtual void showEvent(QShowEvent*);

protected:
  void buildInterface();
  void resizeEvent(QResizeEvent* event);
  void updateTable(const QString& fileName);
  void resizeTable();

public slots:
  void openFileRequested();
  void refreshTable();
  void updateTableView();
  void resizeDataTableColumn(int column, int oldWidth, int newWidth);
  void resizeVariablesTableColumn(int column, int oldWidth, int newWidth);
  void sortSectionChanged(int, Qt::SortOrder);

private:
  DataModel * dataModel_;
  ResizableHeaderlessTableView * tableView_;
  DataModelTableModel * tableModel_;
  ExportableTableView * dataTableView1_;
  ExportableTableView * dataTableView2_;
  QLineEdit * filePathLineEdit_;
  QPalette defaultLineEditPalette_;
  QLabel * sampleSizeLabel_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif