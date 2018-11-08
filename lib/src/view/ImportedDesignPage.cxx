//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
#include "otgui/ImportedDesignPage.hxx"

#include "otgui/HorizontalHeaderViewWithCombobox.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/FileTools.hxx"

#include <QScrollBar>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>

using namespace OT;

namespace OTGUI
{

ImportedDesignPage::ImportedDesignPage(QWidget* parent)
  : QWizardPage(parent)
  , designOfExperiment_()
  , pageValidity_(false)
  , filePathLineEdit_(0)
  , dataPreviewTableView_(0)
  , DOESizeLabel_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void ImportedDesignPage::buildInterface()
{
  setWindowTitle(tr("Import table from file"));

  QGridLayout * mainGridLayout = new QGridLayout(this);

  // file path
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("Data file"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  hboxLayout->addWidget(filePathLineEdit_);

  QToolButton * openFileButton = new QToolButton;
  openFileButton->setText("...");
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  mainGridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

  // file preview
  QGroupBox * groupBox = new QGroupBox(tr("File Preview"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  // DOE size
  QHBoxLayout * sizeLayout = new QHBoxLayout;
  QLabel * sizeLabel = new QLabel(tr("Size") + " : ");
  sizeLayout->addWidget(sizeLabel);
  DOESizeLabel_ = new QLabel(QString::number(0));
  sizeLayout->addWidget(DOESizeLabel_);
  sizeLayout->addStretch();
  gridLayout->addLayout(sizeLayout, 0, 0);

  dataPreviewTableView_ = new ExportableTableView(groupBox);
  gridLayout->addWidget(dataPreviewTableView_, 1, 0, 1, 1);

  mainGridLayout->addWidget(groupBox, 1, 0, 1, 1);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainGridLayout->addWidget(errorMessageLabel_, 2, 0, 1, 1);
}


void ImportedDesignPage::openFileRequested()
{
  QString fileName = QFileDialog::getOpenFileName(this,
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
      setData(fileName);
    }
  }
}


void ImportedDesignPage::setData(const QString& fileName)
{
  filePathLineEdit_->setText(fileName);
  try
  {
    errorMessageLabel_->setText("");
    setTable(fileName);
    pageValidity_ = true;
  }
  catch (std::exception & ex)
  {
    dataPreviewTableView_->setModel(0);
    // DOE size
    DOESizeLabel_->setText("");
    QString message = tr("Impossible to load the file.") + "\n";
    message = QString("<font color=red>%1%2</font>").arg(message).arg(ex.what());
    errorMessageLabel_->setText(message);
    pageValidity_ = false;
  }
}


void ImportedDesignPage::setTable(const QString& fileName)
{
  // set file name
  designOfExperiment_.setFileName(fileName.toLocal8Bit().data());

  // get sample From File
  Sample sample(designOfExperiment_.getSampleFromFile());

  // get input names
  const Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();

  // get inputs columns indices
  Indices columns(designOfExperiment_.getInputColumns());

  // set sample description
  Description desc(sample.getDimension());
  for (UnsignedInteger i = 0; i < columns.getSize(); ++i)
    desc[columns[i]] = inputNames[i];
  sample.setDescription(desc);

  // set table model
  dataPreviewTableView_->setModel(new SampleTableModel(sample, false, dataPreviewTableView_));
  connect(dataPreviewTableView_->model(), SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this, SLOT(columnNameChanged()));

  // set comboboxes items: each of them contains the input Names and an empty item
  QStringList comboBoxItems;
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
    comboBoxItems << QString::fromUtf8(inputNames[i].c_str());
  comboBoxItems << "";

  // set horizontal header view
  QVector<int> columnsWithCombo(sample.getDimension());
  for (int i = 0; i < columnsWithCombo.size(); ++i)
    columnsWithCombo[i] = i;
  HorizontalHeaderViewWithCombobox * header = new HorizontalHeaderViewWithCombobox(comboBoxItems, columnsWithCombo, dataPreviewTableView_);
  dataPreviewTableView_->setHorizontalHeader(header);
  connect(dataPreviewTableView_->horizontalScrollBar(), SIGNAL(valueChanged(int)), header, SLOT(fixComboPositions()));
  dataPreviewTableView_->horizontalHeader()->show();
#if QT_VERSION >= 0x050000
  dataPreviewTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  dataPreviewTableView_->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

  // DOE size
  DOESizeLabel_->setText(QString::number(sample.getSize()));
}


void ImportedDesignPage::columnNameChanged()
{
  const Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();
  // test the unicity of each variable
  Indices columns;
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
    for (int j = 0; j < dataPreviewTableView_->model()->columnCount(); ++j)
      if (inputNames[i] == dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString().toStdString())
      {
        columns.add(j);
        break;
      }
  // test the presence of all variables
  Indices columns2;
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
    for (int j = 0; j < dataPreviewTableView_->model()->columnCount(); ++j)
      if (inputNames[i] == dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString().toStdString())
        columns2.add(j);

  if (columns != columns2)
  {
    QString message = tr("Each variable must be associated with one column.");
    message = QString("<font color=red>%1</font>").arg(message);
    errorMessageLabel_->setText(message);
    pageValidity_ = false;
    return;
  }

  // update the design of experiments
  try
  {
    designOfExperiment_.setInputColumns(columns);
    pageValidity_ = true;
    errorMessageLabel_->setText("");
  }
  catch(InvalidArgumentException & ex)
  {
    QString message = tr("Each variable must be associated with one column.");
    message = QString("<font color=red>%1</font>").arg(message);
    errorMessageLabel_->setText(message);
    pageValidity_ = false;
  }
}


void ImportedDesignPage::initialize(const Analysis& analysis)
{
  ImportedDesignOfExperiment * analysis_ptr = dynamic_cast<ImportedDesignOfExperiment*>(analysis.getImplementation().get());

  // if already an ImportedDesignOfExperiment
  if (analysis_ptr)
  {
    designOfExperiment_ = *analysis_ptr;
    setData(QString::fromUtf8(designOfExperiment_.getFileName().c_str()));
  }
  else
  {
    // create a new analysis
    PhysicalModel physicalModel = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
    designOfExperiment_ = ImportedDesignOfExperiment(analysis.getName(), physicalModel);
  }
}


Analysis ImportedDesignPage::getAnalysis()
{
  return designOfExperiment_;
}


bool ImportedDesignPage::validatePage()
{
  return pageValidity_;
}
}
