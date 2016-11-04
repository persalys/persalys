//                                               -*- C++ -*-
/**
 *  @brief QGroupBox to select outputs
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/OutputsSelectionGroupBox.hxx"

#include <QVBoxLayout>
#include <QComboBox>

using namespace OT;

namespace OTGUI {

OutputsSelectionGroupBox::OutputsSelectionGroupBox(const Description& outputsNames, const Description& interestVariables, QWidget* parent)
  : QGroupBox(tr("Outputs to analyse"), parent)
{
  QVBoxLayout * outputLayout = new QVBoxLayout(this);

  QStringList outputsList;
  for (UnsignedInteger i=0; i<outputsNames.getSize(); ++i)
    outputsList << outputsNames[i].c_str();

  QStringList variablesStringList;
  for (UnsignedInteger i=0; i<interestVariables.getSize(); ++i)
    if (outputsList.contains(interestVariables[i].c_str()))
      variablesStringList << interestVariables[i].c_str();

  if (!variablesStringList.size())
    variablesStringList = outputsList;

  QComboBox * outputsComboBox = new QComboBox;
  outputsListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select outputs") + " --", outputsList, variablesStringList, this);
  connect(outputsListWidget_, SIGNAL(checkedItemsChanged(QStringList)), this, SIGNAL(outputsSelectionChanged(QStringList)));
  outputsComboBox->setModel(outputsListWidget_->model());
  outputsComboBox->setView(outputsListWidget_);
  outputLayout->addWidget(outputsComboBox);
}


QStringList OutputsSelectionGroupBox::getSelectedOutputsNames() const
{
  return outputsListWidget_->getCheckedItemsNames();
}
}