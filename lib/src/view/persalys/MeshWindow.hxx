//                                               -*- C++ -*-
/**
 *  @brief Mesh window
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
#ifndef PERSALYS_MESHWINDOW_HXX
#define PERSALYS_MESHWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/PhysicalModelItem.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/SampleTableModel.hxx"

#include <QTabWidget>
#include <QStandardItem>
#include <QLabel>
#include <QResizeEvent>

namespace PERSALYS
{
class PERSALYS_API MeshWindow : public SubWindow
{
  Q_OBJECT

public:
  MeshWindow(PhysicalModelItem * item, QWidget * parent = 0);

  void buildInterface();
#ifdef PERSALYS_HAVE_PARAVIEW
  void addParaviewWidgetsTabs();
#endif
  void addWidgetsTabs();
  void updatePlot();

protected:
  virtual void resizeEvent(QResizeEvent * event);

public slots:
  void updateTable();
  void updateModel(QStandardItem *item);
  void editMesh();

private:
  PhysicalModelItem * meshItem_;
  QLabel * isRegularLabel_;
  CopyableTableView * tableView_;
  CustomStandardItemModel * tableModel_;
  PlotWidget * meshPlot_;
  SampleTableModel * nodesModel_;
  ExportableTableView * nodesView_;
  QTabWidget * tabWidget_;
};
}
#endif
