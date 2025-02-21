//                                               -*- C++ -*-
/**
 *  @brief Mesh window
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/DataFieldModelItem.hxx"
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
class PERSALYS_VIEW_API MeshWindow : public SubWindow
{
  Q_OBJECT

public:
  MeshWindow(PhysicalModelItem * item, QWidget *parent = nullptr);
  MeshWindow(DataFieldModelItem * item, QWidget *parent = nullptr);

  void buildInterface();
#ifdef PERSALYS_HAVE_PARAVIEW
  void addParaviewWidgetsTabs();
#endif
  void addWidgetsTabs();
  void updatePlot();

protected:
  virtual void resizeEvent(QResizeEvent * event);
  MeshModel getMeshModel() const;

public slots:
  void updateTable();
  void updateModel(QStandardItem *item);
  void editMesh();

private:
  PhysicalModelItem * meshItem_ = nullptr;
  DataFieldModelItem * dataMeshItem_ = nullptr;
  QLabel * isRegularLabel_ = nullptr;
  CopyableTableView * tableView_ = nullptr;
  CustomStandardItemModel * tableModel_ = nullptr;
  PlotWidget * meshPlot_ = nullptr;
  SampleTableModel * nodesModel_ = nullptr;
  ExportableTableView * nodesView_ = nullptr;
  QTabWidget * tabWidget_ = nullptr;
};
}
#endif
