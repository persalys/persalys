//                                               -*- C++ -*-
/**
 *  @brief QWidget to import sample
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/ImportSampleWidget.hxx"

#include "persalys/HorizontalHeaderViewWithCombobox.hxx"
#include "persalys/SampleTableModel.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/QtTools.hxx"

#include <QScrollBar>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>

using namespace OT;

namespace PERSALYS
{

ImportSampleWidget::ImportSampleWidget(QWidget* parent, bool chooseOrder)
  : QWidget(parent)
  , tableValidity_(false)
  , chooseOrder_(chooseOrder)
{
  buildInterface();
}


void ImportSampleWidget::buildInterface()
{
  QGridLayout * mainGridLayout = new QGridLayout(this);
  mainGridLayout->setContentsMargins(0, 0, 0, 0);

  // file path
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("Data file"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  hboxLayout->addWidget(filePathLineEdit_);

  QToolButton * openFileButton = new QToolButton;
  openFileButton->setText("...");
  openFileButton->setToolTip(tr("Search file"));
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  int row = 0;
  mainGridLayout->addLayout(hboxLayout, row++, 0, 1, 3);

  if (chooseOrder_)
  {
    QHBoxLayout * buttonsLayout = new QHBoxLayout;
    orderButtonGroup_ = new QButtonGroup(this);
    QRadioButton * orderButton = new QRadioButton(tr("Columns"));
    orderButtonGroup_->addButton(orderButton, Tools::Columns);
    buttonsLayout->addWidget(orderButton);

    orderButton = new QRadioButton(tr("Rows"));
    orderButtonGroup_->addButton(orderButton, Tools::Rows);
    orderButtonGroup_->button(Tools::Columns)->click();
    orderButtonGroup_->button(0)->setEnabled(QFile(filePathLineEdit_->text()).exists());
    orderButtonGroup_->button(1)->setEnabled(QFile(filePathLineEdit_->text()).exists());
    buttonsLayout->addWidget(orderButton);
    buttonsLayout->addStretch();

    mainGridLayout->addLayout(buttonsLayout, row++, 0, 1, 3);

    connect(orderButtonGroup_, &QButtonGroup::idClicked, [=] (int) {
      setData(filePathLineEdit_->text());
    });

    connect(filePathLineEdit_, &QLineEdit::textChanged, [=] (QString) {
      orderButtonGroup_->button(0)->setEnabled(QFile(filePathLineEdit_->text()).exists());
      orderButtonGroup_->button(1)->setEnabled(QFile(filePathLineEdit_->text()).exists());
    });
  }

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

  mainGridLayout->addWidget(groupBox, row++, 0, 1, 1);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  mainGridLayout->addWidget(errorMessageLabel_, row++, 0, 1, 1);
}


void ImportSampleWidget::openFileRequested()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                     tr("Data to import..."),
                     FileTools::GetCurrentDir(),
                     tr("Data files (*.csv *.txt *.dat)"));

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


void ImportSampleWidget::setData(const QString& fileName)
{
  filePathLineEdit_->setText(fileName);
  try
  {
    errorMessageLabel_->reset();
    emit updateTableRequested(fileName);
    tableValidity_ = true;
  }
  catch (std::exception & ex)
  {
    dataPreviewTableView_->setModel(0);
    // DOE size
    DOESizeLabel_->setText("");
    errorMessageLabel_->setErrorMessage(tr("Impossible to load the file.%1%2").arg("\n").arg(ex.what()));
    tableValidity_ = false;
  }
}


void ImportSampleWidget::updateWidgets(const Sample& fileSample, const Description& variableNames, const Indices& variablecolumns, const Description &comboItems)
{
  Sample sample(fileSample);
  const Description initialDescription(sample.getDescription());

  // set sample description
  Description desc(sample.getDimension());
  for (UnsignedInteger i = 0; i < variablecolumns.getSize(); ++i)
    desc[variablecolumns[i]] = variableNames[i];
  sample.setDescription(desc);

  // set table model
  dataPreviewTableView_->setModel(new SampleTableModel(sample, initialDescription, dataPreviewTableView_));
  connect(dataPreviewTableView_->model(), SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this, SIGNAL(checkColumnsRequested()));

  // set comboboxes items: each of them contains the input Names and an empty item
  QStringList comboBoxItems;
  for (UnsignedInteger i = 0; i < comboItems.getSize(); ++i)
    comboBoxItems << QString::fromUtf8(comboItems[i].c_str());
  comboBoxItems << "";

  // set horizontal header view
  QVector<int> columnsWithCombo(sample.getDimension());
  for (int i = 0; i < columnsWithCombo.size(); ++i)
    columnsWithCombo[i] = i;
  HorizontalHeaderViewWithCombobox * header = new HorizontalHeaderViewWithCombobox(comboBoxItems, columnsWithCombo, dataPreviewTableView_);
  dataPreviewTableView_->setHorizontalHeader(header);
  connect(dataPreviewTableView_->horizontalScrollBar(), SIGNAL(valueChanged(int)), header, SLOT(fixComboPositions()));
  dataPreviewTableView_->horizontalHeader()->show();
  dataPreviewTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

  // DOE size
  DOESizeLabel_->setText(QString::number(sample.getSize()));
}


void ImportSampleWidget::updateWidgets(const Sample& fileSample, const Description& variableNames, const Indices& variablecolumns)
{
  updateWidgets(fileSample, variableNames, variablecolumns, variableNames);
}


Indices ImportSampleWidget::getColumns(const Description &names) const
{
  Indices columns;
  // get the index of each item of names
  QStringList addedVars;
  for (UnsignedInteger i = 0; i < names.getSize(); ++i)
  {
    for (int j = 0; j < dataPreviewTableView_->model()->columnCount(); ++j)
    {
      QString headerName_j(dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString());
      if (names[i] == headerName_j.toStdString())
      {
        columns.add(j);
        addedVars.append(headerName_j);
      }
    }
  }
  if (addedVars.size())
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    if ((UnsignedInteger)QSet<QString>(addedVars.begin(), addedVars.end()).size() != columns.getSize())
#else
    if ((UnsignedInteger)addedVars.toSet().size() != columns.getSize())
#endif
      throw InvalidArgumentException(HERE) << "Each variable must be present only once";
  return columns;
}

Sample ImportSampleWidget::getData() const
{
  SampleTableModel * model = dynamic_cast<SampleTableModel*>(dataPreviewTableView_->model());
  if (model)
    return model->getSample();
  else
    throw InvalidArgumentException(HERE) << "Table model is not a SampleTableModel";
}

Tools::DataOrder ImportSampleWidget::getDataOrder() const
{
  if (orderButtonGroup_)
    return static_cast<Tools::DataOrder>(orderButtonGroup_->checkedId());
  else
    return Tools::Columns;
}
}
