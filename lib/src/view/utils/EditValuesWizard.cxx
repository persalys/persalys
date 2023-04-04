//                                               -*- C++ -*-
/**
 *  @brief QDialog to define values
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/EditValuesWizard.hxx"

#include "persalys/DoubleSpinBox.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/FileTools.hxx"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QDebug>
#include <QtAlgorithms>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <cfloat>
#include <openturns/UserDefined.hxx>

using namespace OT;

namespace PERSALYS
{

EditValuesWizard::EditValuesWizard(QWidget *parent)
  : QWizard(parent)
  , model_(0)
  , proxy_(new QSortFilterProxyModel(this))
  , errorMessageLabel_(new TemporaryLabel)
  , sampleDescription_()
  , valueTable_(new QTableView(this))
  , valueNumber_(new QLabel("0"))
  , removeButton_(0)
{
}


EditValuesWizard::EditValuesWizard(const Sample &values, QWidget *parent)
  : QWizard(parent)
  , model_(new SampleTableModel(values, true, false, Description(), this))
  , proxy_(new QSortFilterProxyModel(this))
  , errorMessageLabel_(new TemporaryLabel)
  , sampleDescription_(values.getDescription())
  , valueTable_(new QTableView(this))
  , valueNumber_(new QLabel("0"))
  , removeButton_(0)
{
  buildInterface();
}


EditValuesWizard::EditValuesWizard(const QString &variableName, const Point &values, QWidget *parent)
  : QWizard(parent)
  , model_(0)
  , proxy_(new QSortFilterProxyModel(this))
  , errorMessageLabel_(new TemporaryLabel)
  , sampleDescription_()
  , valueTable_(new QTableView(this))
  , valueNumber_(new QLabel("0"))
  , removeButton_(0)
{
  Point points(values);
  // remove duplicates
  auto last = std::unique(points.begin(), points.end());
  points.erase(last, points.end());
  Sample sample(points.getSize(), 1);
  std::copy(points.begin(), points.end(), &sample(0, 0));
  sampleDescription_ = Description(1, variableName.toStdString());
  sample.setDescription(sampleDescription_);

  model_ = new SampleTableModel(sample, true, false, Description(), this);

  buildInterface();
}


void EditValuesWizard::buildInterface()
{
  setWindowTitle(tr("Define values"));
  setButtonText(QWizard::FinishButton, tr("Finish"));
  setButtonText(QWizard::CancelButton, tr("Cancel"));
  setOption(QWizard::NoDefaultButton, true);
  setOption(QWizard::NoBackButtonOnStartPage, true);

  proxy_->setSourceModel(model_);
  proxy_->setSortRole(Qt::UserRole);
  valueTable_->setModel(proxy_);
  proxy_->sort(0);
  valueTable_->horizontalHeader()->setStretchLastSection(true);
  valueTable_->verticalHeader()->hide();
  connect(valueTable_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(checkButtons()));

  QVBoxLayout * optionLayout = new QVBoxLayout;

  // number of values Label
  QLabel * label = new QLabel(tr("<b>Number of values:</b>"));
  QHBoxLayout* numberLayout = new QHBoxLayout;
  numberLayout->addWidget(label);
  numberLayout->addWidget(valueNumber_);
  optionLayout->addLayout(numberLayout);

  // add button
  QPushButton * addButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"), this);
  addButton->setToolTip(tr("Add a value"));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addValue()));
  optionLayout->addWidget(addButton);

  // remove button
  removeButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeButton_->setToolTip(tr("Remove the selected values"));
  connect(removeButton_, SIGNAL(clicked()), this, SLOT(removeSelectedValues()));
  optionLayout->addWidget(removeButton_);

  QPushButton * importButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Import..."), this);
  importButton->setToolTip(tr("Import a sample"));
  connect(importButton, SIGNAL(clicked()), this, SLOT(importSample()));
  optionLayout->addWidget(importButton);

  optionLayout->addStretch();

  QWizardPage * page = new QWizardPage;
  QGridLayout * pageLayout = new QGridLayout(page);
  pageLayout->addWidget(valueTable_, 0, 0);
  pageLayout->addLayout(optionLayout, 0, 1);
  pageLayout->addWidget(errorMessageLabel_, 1, 0, 1, 2);

  addPage(page);

  checkButtons();
}


void EditValuesWizard::addValue()
{
  Sample sample(model_->getSample());
  if (sample.getSize())
  {
    Point newpoint(model_->columnCount());
    for (int i = 0; i < model_->columnCount(); ++i)
    {
      const QModelIndex greaterValueIndex(proxy_->mapToSource(proxy_->index(proxy_->rowCount()-1, i)));
      newpoint[i] = model_->data(greaterValueIndex, Qt::UserRole).toDouble() + 1;
    }
    sample.add(newpoint);
  }
  else
    sample = Sample(1, 1);

  if (sampleDescription_.getSize() == sample.getDimension())
    sample.setDescription(sampleDescription_);

  // update table
  model_->updateData(sample);

  check();
}

void EditValuesWizard::importSample()
{
  QWizard * importWizard = new QWizard;
  ImportedDistributionPage * page = new ImportedDistributionPage;
  importWizard->addPage(page);

  if (importWizard->exec()) {
    Sample newSample(page->getData().getSize(), 0);
    // Columns from imported sample
    newSample.stack(page->getData().getMarginal(Description(1, tr("Value").toStdString())));
    const Description columnNames = page->getData().getDescription();
    const Description::const_iterator it = std::find(columnNames.begin(),
                                                     columnNames.end(),
                                                     tr("Weight").toStdString());
    // Widget has been used to specify probabilities
    if (it != columnNames.end())
      newSample.stack(page->getData().getMarginal(Description(1, tr("Weight").toStdString())));
    else
      // Columns with associated uniform probabilities
      newSample.stack(Sample(page->getData().getSize(), Point(1, 1.)));
    Description description(2);
    description[0] = tr("Value").toStdString();
    description[1] = tr("Weight").toStdString();
    newSample.setDescription(description);

    model_->updateData(newSample);
    check();
  }
}


void EditValuesWizard::checkButtons()
{
  removeButton_->setEnabled(!valueTable_->selectionModel()->selection().empty());
}


void EditValuesWizard::removeSelectedValues()
{
  QList<int> selectedRow;
  // retrieve rows
  foreach (QModelIndex index, valueTable_->selectionModel()->selectedIndexes())
    selectedRow << proxy_->mapToSource(index).row();

  // sort
  std::sort(selectedRow.begin(), selectedRow.end(), std::greater<int>());

  // remove
  Sample sample(model_->getSample());
  foreach (int row, selectedRow)
  {
    Point point(sample[row]);
    sample.erase(row);
  }
  model_->updateData(sample);

  check();
}


Point EditValuesWizard::getValues(const UnsignedInteger index) const
{
  return model_->getSample().getMarginal(index).asPoint();
}


void EditValuesWizard::check()
{
  proxy_->sort(0);
  valueNumber_->setText(QString::number(model_->getSample().getSize()));
  checkButtons();
}


bool EditValuesWizard::validateCurrentPage()
{
  if (model_->getSample().getSize() < 2)
  {
    errorMessageLabel_->setErrorMessage(tr("Define at least two values"));
    return false;
  }
  return true;
}


UserDefinedWizard::UserDefinedWizard(const Distribution::PointWithDescriptionCollection &parameters, QWidget *parent)
  : EditValuesWizard(parent)
{
  Q_ASSERT(parameters.getSize() == 2);

  const UnsignedInteger nbPoints = parameters[0].getSize();
  Q_ASSERT(nbPoints);

  Sample sample(nbPoints, 2);
  for (UnsignedInteger i = 0; i < 2; ++i)
  {
    Q_ASSERT(parameters[i].getSize() == nbPoints);
    for (UnsignedInteger j = 0; j < nbPoints; ++j)
      sample(j, i) = parameters[i][j];
  }
  Description description(2);
  description[0] = tr("Value").toStdString();
  description[1] = tr("Weight").toStdString();
  sample.setDescription(description);
  model_ = new WeightTableModel(sample, this);
  connect(model_, SIGNAL(dataChanged(QModelIndex, QModelIndex)), errorMessageLabel_, SLOT(reset()));
  connect(model_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));

  buildInterface();
}


void UserDefinedWizard::addValue()
{
  Point newpoint(model_->columnCount());
  const QModelIndex greaterValueIndex(proxy_->mapToSource(proxy_->index(proxy_->rowCount()-1, 0)));
  newpoint[0] = model_->data(greaterValueIndex, Qt::UserRole).toDouble() + 1;

  Sample sample(model_->getSample());
  Point proba(sample.getMarginal(1).asPoint());
  // if sum < 1 : p = 1 - sum
  // if sum >= 1 : p = 0
  newpoint[1] = 1.0 - std::min(std::accumulate(proba.begin(), proba.end(), 0.0), 1.0);

  sample.add(newpoint);
  model_->updateData(sample);

  check();
}


Distribution UserDefinedWizard::getDistribution() const
{
  return UserDefined(model_->getSample().getMarginal(0), getValues(1));
}


bool UserDefinedWizard::validateCurrentPage()
{
  errorMessageLabel_->reset();
  Sample sample(model_->getSample());
  if (sample.getSize() < 2)
  {
    errorMessageLabel_->setErrorMessage(tr("Define at least two values"));
    return false;
  }
  return true;
}

ImportedDistributionPage::ImportedDistributionPage(QWidget *parent)
  : QWizardPage(parent)
{
  QVBoxLayout * pageLayout = new QVBoxLayout(this);
  sampleWidget_ = new ImportSampleWidget;
  pageLayout->addWidget(sampleWidget_);

  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_);

  connect(sampleWidget_, &ImportSampleWidget::updateTableRequested, [=](const QString & fileName) {
    errorMessageLabel_->setText("");
    Sample sample = Tools::ImportSample(fileName.toStdString());
    Indices allIndices = Indices(sample.getDimension());
    allIndices.fill(0, 1);
    Description description(2);
    description[0] = tr("Value").toStdString();
    description[1] = tr("Weight").toStdString();
    sampleWidget_->updateWidgets(sample, sample.getDescription(), allIndices, description);});
}

bool ImportedDistributionPage::validatePage()
{
  const Description desc = getData().getDescription();
  for (UnsignedInteger i=0; i<desc.getSize()-1; ++i)
  {
    for (UnsignedInteger j=i+1; j<desc.getSize(); ++j)
    {
      if (desc[i] == desc[j] && desc[i] != "")
      {
        errorMessageLabel_->setErrorMessage(tr("Values and weights must be associated with one column"));
        return false;
      }
    }
  }
  return true;
}
}
