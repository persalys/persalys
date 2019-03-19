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
#ifndef OTGUI_MESHWINDOW_HXX
#define OTGUI_MESHWINDOW_HXX

#include "otgui/SubWindow.hxx"
#include "otgui/MeshItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"

#include <QTabWidget>
#include <QStandardItem>
#include <QLabel>

namespace OTGUI
{
class OTGUI_API MeshWindow : public SubWindow
{
  Q_OBJECT

public:
  MeshWindow(MeshItem * item, QWidget * parent = 0);

  void buildInterface();
#ifdef OTGUI_HAVE_PARAVIEW
  void addParaviewWidgetsTabs();
#endif
  void addWidgetsTabs();
  void updatePlot();

public slots:
  void updateTable();
  void updateModel(QStandardItem *item);
  void editMesh();

private:
  MeshItem * meshItem_;
  QLabel * isRegularLabel_;
  CustomStandardItemModel * tableModel_;
  PlotWidget * meshPlot_;
  SampleTableModel * nodesModel_;
  ExportableTableView * nodesView_;
  QTabWidget * tabWidget_;
};
}
#endif
