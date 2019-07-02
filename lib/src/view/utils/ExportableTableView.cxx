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
#include "persalys/ExportableTableView.hxx"

#include "persalys/SampleTableModel.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/FileTools.hxx"

#include <QMenu>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

namespace PERSALYS
{

ExportableTableView::ExportableTableView(QWidget* parent)
  : CopyableTableView(parent)
  , exportAction_(0)
  , exportableAsImageAction_(0)
{
  // default action
  exportAction_ = new QAction(QIcon(":/images/document-export-table.png"), tr("Export"), this);
  exportAction_->setStatusTip(tr("Export the data"));
  connect(exportAction_, SIGNAL(triggered()), this, SLOT(exportData()));

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


void ExportableTableView::setExportableAsImage(const bool exportable)
{
  if (exportable)
  {
    exportableAsImageAction_ = new QAction(QIcon(":/images/document-export-table.png"), tr("Export as Image"), this);
    exportableAsImageAction_->setStatusTip(tr("Export table as image"));
    connect(exportableAsImageAction_, SIGNAL(triggered()), this, SLOT(exportImage()));
  }
}


// show the context menu when right clicking
void ExportableTableView::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(exportAction_);
  if (exportableAsImageAction_)
    contextMenu->addAction(exportableAsImageAction_);
  contextMenu->popup(this->mapToGlobal(pos));
}


void ExportableTableView::exportData()
{
  OT::Sample sample;
  QString text;

  if (dynamic_cast<SampleTableModel*>(model()))
  {
    sample = dynamic_cast<SampleTableModel*>(model())->getSample();
  }
  else if (dynamic_cast<QSortFilterProxyModel*>(model()))
  {
    QAbstractItemModel * sourceModel = dynamic_cast<QSortFilterProxyModel*>(model())->sourceModel();
    if (dynamic_cast<SampleTableModel*>(sourceModel))
      sample = dynamic_cast<SampleTableModel*>(sourceModel)->getSample();
  }
  else if (dynamic_cast<CustomStandardItemModel*>(model()))
  {
    text = dynamic_cast<CustomStandardItemModel*>(model())->getFormattedText();
  }

  if (sample.getSize())
    FileTools::ExportData(sample, this);
  else if (!text.isEmpty())
    FileTools::ExportData(text, this);
  else
    throw SimpleException(tr("Internal exception: cannot get the sample"));
}


void ExportableTableView::exportImage()
{
  int x1, y1, x2, y2;
  getContentsMargins(&x1, &y1, &x2, &y2);
  const int w = horizontalHeader()->length() + verticalHeader()->width();
  const int h = verticalHeader()->length() + horizontalHeader()->height();

  QImage image(QSize(w + x1 + x2, h + y1 + y2), QImage::Format_ARGB32_Premultiplied);
  render(&image);

  FileTools::ExportImage(image, this);
}


QWidget * ExportableTableView::GetSampleTableViewWidget(Item* item, const OT::Sample& sample)
{
  // create the Table
  ExportableTableView * tableView = new ExportableTableView;
  connect(item, SIGNAL(dataExportRequested()), tableView, SLOT(exportData()));
  tableView->setSortingEnabled(true);

  SampleTableModel * tableModel = new SampleTableModel(sample, tableView);
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tableView);
  proxyModel->setSourceModel(tableModel);
  proxyModel->setSortRole(Qt::UserRole);

  tableView->setModel(proxyModel);
  tableView->sortByColumn(0, Qt::AscendingOrder);

  // create the Widget
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QHBoxLayout * hLayout = new QHBoxLayout;

  // - sample size
  QLabel * sizeLabel = new QLabel(tr("Size") + " : " + QString::number(sample.getSize()));
  hLayout->addWidget(sizeLabel);
  hLayout->addStretch();

  // - export button
  QPushButton * exportButton = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  hLayout->addWidget(exportButton);
  connect(exportButton, SIGNAL(clicked()), tableView, SLOT(exportData()));

  mainLayout->addLayout(hLayout);

  // - table
  mainLayout->addWidget(tableView);

  return mainWidget;
}
}
