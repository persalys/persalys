//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
#include "otgui/ImportDesignOfExperimentPage.hxx"

#include "otgui/HorizontalHeaderViewWithCombobox.hxx"
#include "otgui/SampleTableModel.hxx"

#include <QScrollBar>

using namespace OT;

namespace OTGUI {

ImportDesignOfExperimentPage::ImportDesignOfExperimentPage(const DesignOfExperiment& designOfExperiment, QWidget* parent)
  : ImportDataPage(parent)
{
  if (designOfExperiment.getImplementation()->getClassName() == "FromFileDesignOfExperiment")
  {
    designOfExperiment_ = *dynamic_cast<const FromFileDesignOfExperiment*>(&*designOfExperiment.getImplementation());
    if (designOfExperiment_.getFileName().size())
      setData(QString::fromUtf8(designOfExperiment_.getFileName().c_str()));
  }
  else
  {
    designOfExperiment_ = FromFileDesignOfExperiment(designOfExperiment.getName(), designOfExperiment.getPhysicalModel());
  }
}


void ImportDesignOfExperimentPage::setTable(const QString& fileName)
{
  // set file name
  designOfExperiment_.setFileName(fileName.toLocal8Bit().data());

  // check sample From File
  Sample sample(designOfExperiment_.getSampleFromFile());
  if (!designOfExperiment_.getInputColumns().check(sample.getDimension()-1))
    throw InvalidArgumentException(HERE) << tr("Impossible to load sample marginals").toLocal8Bit().data();

  const Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();

  if (sample.getDimension() < inputNames.getSize())
    throw InvalidArgumentException(HERE) << tr("The file contains a sample with a dimension inferior to the number of inputs of the physical model:").toLocal8Bit().data()
                                         << " " << inputNames.getSize();

  // set inputs columns indices
  Indices columns(designOfExperiment_.getInputColumns());
  if (!columns.getSize())
  {
    columns = Indices(inputNames.getSize());
    columns.fill();
  }
  // reset input sample
  designOfExperiment_.setInputColumns(columns);

  // set sample description
  Description desc(sample.getDimension());
  for (UnsignedInteger i=0; i<columns.getSize(); ++i)
    desc[columns[i]] = inputNames[i];
  sample.setDescription(desc);

  // set table model
  dataPreviewTableView_->setModel(new SampleTableModel(sample, dataPreviewTableView_));
  connect(dataPreviewTableView_->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(columnNameChanged()));

  // set comboboxes items: each of them contains the input Names and an empty item
  QStringList comboBoxItems;
  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
    comboBoxItems << QString::fromUtf8(inputNames[i].c_str());
  comboBoxItems << "";

  // set horizontal header view
  QVector<int> columnsWithCombo(sample.getDimension());
  for (int i=0; i<columnsWithCombo.size(); ++i)
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
}


void ImportDesignOfExperimentPage::columnNameChanged()
{
  const Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();
  // test the unicity of each variable
  Indices columns;
  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
    for (int j=0; j<dataPreviewTableView_->model()->columnCount(); ++j)
      if (inputNames[i] == dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString().toStdString())
      {
        columns.add(j);
        break;
      }
  // test the presence of all variables
  Indices columns2;
  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
    for (int j=0; j<dataPreviewTableView_->model()->columnCount(); ++j)
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


bool ImportDesignOfExperimentPage::validatePage()
{
  if (pageValidity_)
    emit designOfExperimentChanged(designOfExperiment_);
  return pageValidity_;
}
}