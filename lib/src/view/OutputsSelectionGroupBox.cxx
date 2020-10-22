//                                               -*- C++ -*-
/**
 *  @brief QGroupBox to select outputs
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/OutputsSelectionGroupBox.hxx"

#include <QVBoxLayout>
#include <QScrollArea>

using namespace OT;

namespace PERSALYS
{

OutputsSelectionGroupBox::OutputsSelectionGroupBox(QWidget* parent)
  : QGroupBox(tr("Outputs of interest"), parent)
  , pluralText_(true)
  , namesLabel_(new QLabel)
  , outputsListWidget_(0)
  , outputsComboBox_(0)
{
  QVBoxLayout * outputLayout = new QVBoxLayout(this);

  // custom combobox to choose output of interest
  outputsComboBox_ = new TitledComboBox("-- " + tr("Select outputs") + " --");
  outputLayout->addWidget(outputsComboBox_);
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidget(namesLabel_);
  scrollArea->setFixedHeight(namesLabel_->height()+20);
  outputLayout->addWidget(scrollArea);
}


OutputsSelectionGroupBox::OutputsSelectionGroupBox(bool pluralText, QWidget* parent)
  : QGroupBox((pluralText ? tr("Outputs of interest") : tr("Output of interest")), parent)
  , pluralText_(pluralText)
  , namesLabel_(new QLabel)
  , outputsListWidget_(0)
  , outputsComboBox_(0)
{
  QVBoxLayout * outputLayout = new QVBoxLayout(this);

  // custom combobox to choose output of interest
  outputsComboBox_ = new TitledComboBox("-- " + (pluralText_ ? tr("Select outputs") : tr("Select output")) + " --");
  outputLayout->addWidget(outputsComboBox_);
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidget(namesLabel_);
  scrollArea->setFixedHeight(namesLabel_->height()+20);
  outputLayout->addWidget(scrollArea);
}


OutputsSelectionGroupBox::OutputsSelectionGroupBox(const Description& outputsNames, const Description& interestVariables, QWidget* parent)
  : QGroupBox(tr("Outputs of interest"), parent)
  , pluralText_(true)
  , namesLabel_(new QLabel)
  , outputsListWidget_(0)
  , outputsComboBox_(0)
{
  QVBoxLayout * outputLayout = new QVBoxLayout(this);

  // custom combobox to choose output of interest
  outputsComboBox_ = new TitledComboBox("-- " + tr("Select outputs") + " --");
  outputLayout->addWidget(outputsComboBox_);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidget(namesLabel_);
  scrollArea->setFixedHeight(namesLabel_->height()+20);
  outputLayout->addWidget(scrollArea);
  updateComboBoxModel(outputsNames, interestVariables);
}


QStringList OutputsSelectionGroupBox::getSelectedOutputsNames() const
{
  if (outputsListWidget_)
    return outputsListWidget_->getCheckedItemNames();
  return QStringList();
}


void OutputsSelectionGroupBox::updateComboBoxModel(const Description& outputsNames, const Description& interestVariables)
{
  if (outputsListWidget_)
  {
    outputsComboBox_->clear();
    outputsListWidget_ = 0;
  }

  QStringList outputsList;
  for (UnsignedInteger i = 0; i < outputsNames.getSize(); ++i)
    outputsList << QString::fromUtf8(outputsNames[i].c_str());

  QStringList variablesStringList;
  for (UnsignedInteger i = 0; i < interestVariables.getSize(); ++i)
    if (outputsList.contains(QString::fromUtf8(interestVariables[i].c_str())))
      variablesStringList << QString::fromUtf8(interestVariables[i].c_str());

  if (!variablesStringList.size())
    variablesStringList = outputsList;
  outputsListWidget_ = new ListWidgetWithCheckBox("-- " + (pluralText_ ? tr("Select outputs") : tr("Select output")) + " --", outputsList, variablesStringList, this);

  connect(outputsListWidget_, SIGNAL(checkedItemsChanged(QStringList)), this, SIGNAL(outputsSelectionChanged(QStringList)));
  connect(outputsListWidget_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(updateLabel(QStringList)));
  outputsComboBox_->setModel(outputsListWidget_->model());
  outputsComboBox_->setView(outputsListWidget_);

  namesLabel_->setText(variablesStringList.join(", "));
  namesLabel_->adjustSize();
}


void OutputsSelectionGroupBox::updateLabel(const QStringList& variables)
{
  namesLabel_->setText(variables.join(", "));
  namesLabel_->adjustSize();
}
}
