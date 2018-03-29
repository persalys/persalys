//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/DataModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/FileTools.hxx"
#include "otgui/LineEditWithQValidatorDelegate.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/QtTools.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QScrollBar>
#include <QApplication>

using namespace OT;

namespace OTGUI
{

DataModelWindow::DataModelWindow(DataModelDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , dataModel_(0)
  , tableView_(0)
  , tableModel_(0)
  , dataTableView1_(0)
  , dataTableView2_(0)
  , filePathLineEdit_(0)
  , defaultLineEditPalette_()
  , sampleSizeLabel_(0)
  , errorMessageLabel_(0)
{
  dataModel_ = dynamic_cast<DataModel*>(item->getDesignOfExperiment().getImplementation().get());
  if (!dataModel_)
    throw InvalidArgumentException(HERE) << "DataModelWindow: the design of experiments must be a DataModel";

  connect(item, SIGNAL(variablesChanged()), this, SLOT(updateTableView()));

  buildInterface();
}


DataModelWindow::~DataModelWindow()
{
  dataModel_ = 0;
}


void DataModelWindow::buildInterface()
{
  setWindowTitle(tr("Data model definition"));

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainGridLayout = new QGridLayout(mainWidget);

  // first row
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("Data file"));
  hboxLayout->addWidget(label);

  // file path line edit
  filePathLineEdit_ = new QLineEdit;
  defaultLineEditPalette_ = filePathLineEdit_->palette();
  filePathLineEdit_->setReadOnly(true);
  filePathLineEdit_->setText(dataModel_->getFileName().c_str());
  hboxLayout->addWidget(filePathLineEdit_);

  // open file button
  QToolButton * openFileButton = new QToolButton;
  openFileButton->setText("...");
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  // reload button
  QToolButton * reloadButton = new QToolButton;
  reloadButton->setIcon(QIcon(":/images/view-refresh.png"));
  connect(reloadButton, SIGNAL(clicked()), this, SLOT(refreshTable()));
  hboxLayout->addWidget(reloadButton);

  mainGridLayout->addLayout(hboxLayout, 0, 0);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  mainGridLayout->addWidget(errorMessageLabel_, 1, 0);

  // variables table
  QGroupBox * groupBox = new QGroupBox(tr("Variables"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  tableView_ = new ResizableHeaderlessTableView;
  tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  tableView_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  tableView_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  tableView_->setItemDelegateForRow(0, new LineEditWithQValidatorDelegate(false, tableView_));
  tableView_->setItemDelegateForRow(1, new ComboBoxDelegate(tableView_));

  tableModel_ = new DataModelTableModel(dataModel_, tableView_);
  tableView_->setModel(tableModel_);

  gridLayout->addWidget(tableView_, 0, 0);

  mainGridLayout->addWidget(groupBox, 2, 0);

  // file preview
  groupBox = new QGroupBox(tr("Sample"));
  gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  // sample size
  QGridLayout * sizeLayout = new QGridLayout;
  QLabel * sizeLabel = new QLabel(tr("Size") + " : ");
  sizeLayout->addWidget(sizeLabel, 0, 0);
  sampleSizeLabel_ = new QLabel;
  sizeLayout->addWidget(sampleSizeLabel_, 0, 1);
  sizeLayout->setColumnStretch(1, 1);
  sizeLayout->setSizeConstraint(QLayout::SetFixedSize);
  gridLayout->addLayout(sizeLayout, 0, 0);

  // - data table model
  SampleTableModel * dataTableModel = new SampleTableModel(Sample(), this);

  // - data QSortFilterProxyModel
  SampleTableProxyModel * proxyModel = new SampleTableProxyModel(this);
  proxyModel->setSourceModel(dataTableModel);
  proxyModel->setSortRole(Qt::UserRole);

  // - data table view
  QFrame * frame = new QFrame;
  QHBoxLayout * tableLayout = new QHBoxLayout(frame);
  tableLayout->setMargin(0);

  // -- first part: row ID
  dataTableView1_ = new ExportableTableView;
  dataTableView1_->setModel(proxyModel);
  dataTableView1_->setSelectionBehavior(QAbstractItemView::SelectRows);
  dataTableView1_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  dataTableView1_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  dataTableView1_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  dataTableView1_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  dataTableView1_->setSortingEnabled(true);
  tableLayout->addWidget(dataTableView1_);

  // -- second part: sample
  dataTableView2_ = new ExportableTableView;
  dataTableView2_->setModel(proxyModel);
  dataTableView2_->setFrameShape(QFrame::NoFrame);
  dataTableView2_->setSelectionBehavior(QAbstractItemView::SelectRows);
  dataTableView2_->setSelectionModel(dataTableView1_->selectionModel());
  dataTableView2_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  dataTableView2_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  dataTableView2_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  dataTableView2_->verticalHeader()->hide();
  dataTableView2_->setSortingEnabled(true);
  tableLayout->addWidget(dataTableView2_);

  // use QTableView style
  frame->setFrameShadow(dataTableView1_->frameShadow());
  frame->setFrameStyle(dataTableView1_->frameStyle());
  frame->setFrameShape(dataTableView1_->frameShape());
  frame->setLineWidth(dataTableView1_->frameWidth());
  dataTableView1_->setFrameShape(QFrame::NoFrame);

  tableLayout->setSpacing(0);
  gridLayout->addWidget(frame, 1, 0);
  mainGridLayout->addWidget(groupBox, 3, 0);
  mainGridLayout->setRowStretch(3, 10);

  // fix Windows color of selection if no focus (==Inactive status)
  QPalette p = dataTableView2_->palette();
  p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
  p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
  dataTableView2_->setPalette(p);
  dataTableView1_->setPalette(p);

  // connections
  connect(tableModel_, SIGNAL(sampleChanged(OT::Sample)), dataTableModel, SLOT(updateData(OT::Sample)));
  connect(tableModel_, SIGNAL(sampleDescriptionChanged(OT::Description)), dataTableModel, SLOT(updateHeaderData(OT::Description)));
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setErrorMessage(QString)));
  connect(tableModel_, SIGNAL(temporaryErrorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));

  connect(tableView_->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(resizeDataTableColumn(int, int, int)));
  connect(tableView_->horizontalScrollBar(), SIGNAL(valueChanged(int)), dataTableView2_->horizontalScrollBar(), SLOT(setValue(int)));

  connect(dataTableView1_->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(sortSectionChanged(int, Qt::SortOrder)));
  connect(dataTableView1_->verticalScrollBar(), SIGNAL(valueChanged(int)), dataTableView2_->verticalScrollBar(), SLOT(setValue(int)));

  connect(dataTableView2_->horizontalScrollBar(), SIGNAL(valueChanged(int)), tableView_->horizontalScrollBar(), SLOT(setValue(int)));
  connect(dataTableView2_->verticalScrollBar(), SIGNAL(valueChanged(int)), dataTableView1_->verticalScrollBar(), SLOT(setValue(int)));
  connect(dataTableView2_->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(resizeVariablesTableColumn(int, int, int)));
  connect(dataTableView2_->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(sortSectionChanged(int, Qt::SortOrder)));

  // fill tables
  updateTableView();

  setWidget(mainWidget);
}


void DataModelWindow::resizeEvent(QResizeEvent* event)
{
  OTguiSubWindow::resizeEvent(event);
  if (isVisible())
    resizeTable();
}


void DataModelWindow::resizeTable()
{
  int h = tableView_->verticalHeader()->length() + tableView_->horizontalHeader()->height();
  if (tableView_->horizontalScrollBar()->maximum())
    h += tableView_->horizontalScrollBar()->sizeHint().height();
  int x1, y1, x2, y2;
  tableView_->getContentsMargins(&x1, &y1, &x2, &y2);
  tableView_->setFixedHeight(h + y1 + y2);
}


void DataModelWindow::sortSectionChanged(int section, Qt::SortOrder order)
{
  if (section)
  {
    SignalBlocker blocker(dataTableView1_->horizontalHeader());
    dataTableView1_->horizontalHeader()->setSortIndicator(section, order);
  }
  else
  {
    SignalBlocker blocker(dataTableView2_->horizontalHeader());
    dataTableView2_->horizontalHeader()->setSortIndicator(section, order);
  }
}


void DataModelWindow::resizeDataTableColumn(int column, int /*oldWidth*/, int newWidth)
{
  dataTableView2_->horizontalHeader()->resizeSection(column + 1, newWidth);
}


void DataModelWindow::resizeVariablesTableColumn(int column, int /*oldWidth*/, int newWidth)
{
  tableView_->horizontalHeader()->resizeSection(column - 1, newWidth);
}


void DataModelWindow::updateTable(const QString& fileName)
{
  // re-initialization
  errorMessageLabel_->reset();

  // try to retrieve data from the selected file
  try
  {
    // update file name
    dataModel_->setFileName(fileName.toUtf8().data());
    filePathLineEdit_->setText(fileName);
  }
  catch (std::exception& ex)
  {
    QMessageBox::warning(this,
                         tr("Warning"),
                         tr("Cannot update the table.\n%1").arg(ex.what()));
  }
}


void DataModelWindow::openFileRequested()
{
  const QString fileName = QFileDialog::getOpenFileName(this,
                           tr("Data to import..."),
                           FileTools::GetCurrentDir(),
                           tr("Data files (*.csv *.txt)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    FileTools::SetCurrentDir(fileName);

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this,
                           tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      updateTable(fileName);
    }
  }
}


void DataModelWindow::refreshTable()
{
  if (!dataModel_->getFileName().empty())
  {
    updateTable(QString::fromUtf8(dataModel_->getFileName().c_str()));
  }
}


void DataModelWindow::updateTableView()
{
  if (!dataModel_->getSample().getSize())
  {
    resizeTable();
    return;
  }

  // update variables table
  tableModel_->updateData();

  dataTableView1_->sortByColumn(0, Qt::AscendingOrder);

  if (dataModel_->getSampleFromFile().getSize())
  {
    // use comboboxes to define the variable type
    for (int i = 0; i < tableModel_->columnCount(); ++i)
      tableView_->openPersistentEditor(tableModel_->index(1, i));

    // if first time here
    if (!dynamic_cast<CheckableHeaderView*>(tableView_->verticalHeader()))
    {
      // make tableView_ checkable if not done yet
      // table header view
      CheckableHeaderView * tableHeaderView = new CheckableHeaderView(Qt::Vertical);
      tableHeaderView->setSectionResizeMode(QHeaderView::Fixed);
      tableView_->setVerticalHeader(tableHeaderView);
      tableView_->verticalHeader()->show();

      // use a LineEditWithQValidatorDelegate with an offset to see the checkbox
      tableView_->setItemDelegateForRow(0, new LineEditWithQValidatorDelegate(true, tableView_));
    }
  }

  // hide data table columns if not done yet
  if (!dataTableView2_->isColumnHidden(0))
  {
    dataTableView2_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // fix bug on Windows (possible to resize the hidden column!)
    dataTableView2_->hideColumn(0);
  }
  for (int i = 1; i < dataTableView1_->model()->columnCount(); ++i)
    if (!dataTableView1_->isColumnHidden(i))
      dataTableView1_->hideColumn(i);

  // resize correctly tables to have their columns face to face

  // - get dataTableView1_ width
  dataTableView1_->resizeColumnToContents(0);
  int w = dataTableView1_->horizontalHeader()->length() + dataTableView1_->verticalHeader()->sizeHint().width();
  int x1, y1, x2, y2;
  dataTableView1_->getContentsMargins(&x1, &y1, &x2, &y2);
  w += x1 + x2;

  // - get optimal tableView_ vertical header width
  const int optiHeaderWidth = std::max(w, tableView_->verticalHeader()->minimumWidth());

  dataTableView1_->horizontalHeader()->resizeSection(0, optiHeaderWidth - dataTableView1_->verticalHeader()->sizeHint().width());
  dataTableView1_->setFixedWidth(optiHeaderWidth);

  tableView_->verticalHeader()->setFixedWidth(optiHeaderWidth);

  // - hack to avoid problem of column resizing
  tableView_->horizontalHeader()->show();
  tableView_->horizontalHeader()->hide();

  resizeTable();

  // update sample size label
  sampleSizeLabel_->setText(QString::number(dataModel_->getSample().getSize()));
}
}
