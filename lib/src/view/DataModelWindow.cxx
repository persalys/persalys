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
#include "otgui/DataModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>

using namespace OT;

namespace OTGUI {

DataModelWindow::DataModelWindow(DataModelDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , dataModel_(0)
  , filePathLineEdit_(0)
  , defaultLineEditPalette_()
  , reloadButton_(0)
  , dataTableView_(0)
  , dataTableModel_(0)
  , proxyModel_(0)
{
  dataModel_ = dynamic_cast<DataModel*>(&*item->getDesignOfExperiment().getImplementation());
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
  reloadButton_ = new QToolButton;
  reloadButton_->setEnabled(false);
  reloadButton_->setIcon(QIcon(":/images/view-refresh.png"));
  connect(reloadButton_, SIGNAL(clicked()), this, SLOT(refreshTable()));
  hboxLayout->addWidget(reloadButton_);

  mainGridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainGridLayout->addWidget(errorMessageLabel_, 2, 0, 1, 1);

  // file preview
  QGroupBox * groupBox = new QGroupBox(tr("Sample"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  dataTableView_ = new ExportableTableView(groupBox);
  dataTableView_->setSortingEnabled(true);
  dataTableView_->setEditTriggers(QTableView::AllEditTriggers);
  const QStringList comboBoxItems = QStringList() << tr("Input") << tr("Output") << tr("Disable");
  dataTableView_->setItemDelegateForRow(1, new ComboBoxDelegate(comboBoxItems));

  gridLayout->addWidget(dataTableView_, 0, 0, 1, 1);

  mainGridLayout->addWidget(groupBox, 1, 0, 1, 1);

  // if the model has a sample: fill the table
  if (dataModel_->getSample().getSize())
  {
    reloadButton_->setEnabled(true);

    // table view
    updateTableView(dataModel_->getSample());

    // table span
    const UnsignedInteger nbInputs = dataModel_->getInputSample().getSize() ? dataModel_->getInputSample().getDimension() : 0;
    if (nbInputs > 1)
      dataTableView_->setSpan(1, 0, 1, nbInputs);

    if (dataModel_->getOutputSample().getSize() && dataModel_->getOutputSample().getDimension() > 1)
      dataTableView_->setSpan(1, nbInputs, 1, dataModel_->getSample().getDimension());
  }

  setWidget(mainWidget);
}


void DataModelWindow::importSample(const QString& fileName)
{
  filePathLineEdit_->setText(fileName);
  updateTable(fileName);
}


void DataModelWindow::updateTable(const QString& fileName)
{
  // re-initialization
  errorMessageLabel_->setText("");
  filePathLineEdit_->setPalette(defaultLineEditPalette_);

  // try to retrieve data from the selected file
  try
  {
    // update file name
    dataModel_->setFileName(fileName.toLocal8Bit().data());

    // update table view
    updateTableView();
  }
  catch (std::exception& ex)
  {
    // set error message
    QString message = tr("Impossible to update the table.") + "\n";
    message = QString("<font color=red>%1%2</font>").arg(message).arg(ex.what());
    errorMessageLabel_->setText(message);

    // file path in red
    QPalette palette = filePathLineEdit_->palette();
    palette.setColor(QPalette::Text, Qt::red);
    filePathLineEdit_->setPalette(palette);
  }
  reloadButton_->setEnabled(false);
}


void DataModelWindow::openFileRequested()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  const QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Data to import..."),
                                                        currentDir,
                                                        tr("Data files (*.csv *.txt)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this,
                           tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      importSample(fileName);
    }
  }
}


void DataModelWindow::refreshTable()
{
  if (!dataModel_->getFileName().empty())
  {
    if (sender())
      if (sender()->isWidgetType())
        qobject_cast<QWidget*>(sender())->setEnabled(false);
    updateTable(QString::fromUtf8(dataModel_->getFileName().c_str()));
  }
}


void DataModelWindow::updateTableView(const Sample& sample)
{
  // set table model
  if (dataTableModel_)
    delete dataTableModel_;
  if (proxyModel_)
    delete proxyModel_;

  Sample fullSample(sample);
  if (!sample.getSize())
    fullSample = dataModel_->getSampleFromFile();

  dataTableModel_ = new DataModelTableModel(fullSample, dataModel_, (sample.getSize() < 1), dataTableView_);
  connect(dataTableModel_, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  connect(dataTableModel_, SIGNAL(temporaryErrorMessageChanged(QString)), this, SLOT(setTemporaryErrorMessage(QString)));

  proxyModel_ = new DataModelProxModel(dataTableView_);
  proxyModel_->setSourceModel(dataTableModel_);

  // set table view
  dataTableView_->setModel(proxyModel_);
  dataTableView_->sortByColumn(0, Qt::AscendingOrder);
  dataTableView_->clearSpans();

  if (sample.getSize() < 1)
    for (int i = 0; i < proxyModel_->columnCount(); ++i)
      dataTableView_->openPersistentEditor(proxyModel_->index(1, i));
}
}
