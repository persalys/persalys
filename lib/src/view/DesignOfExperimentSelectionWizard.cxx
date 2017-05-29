//                                               -*- C++ -*-
/**
 *  @brief QWizard to select a design of experiment
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
#include "otgui/DesignOfExperimentSelectionWizard.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

namespace OTGUI {

DesignOfExperimentSelectionWizard::DesignOfExperimentSelectionWizard(QList< QStandardItem* > listDOE)
  : OTguiWizard()
  , comboBox_(new QComboBox)
  , comboModel_(new QStandardItemModel)
{
  setWindowTitle(tr("Design of experiment selection"));
  resize(600, 300);

  QWizardPage * page = new QWizardPage(this); // create a class QWidget with the comboBox_ to reuse it in metamodel wizard
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  // label
  QLabel * text = new QLabel(tr("Select the design of experiment to be evaluated"));
  text->setWordWrap(true);
  text->setStyleSheet("font: bold");
  pageLayout->addWidget(text);

  // combo box
  for (int i=0; i<listDOE.count(); ++i)
  {
    comboModel_->appendRow(listDOE[i]);
  }
  comboBox_->setModel(comboModel_);
  pageLayout->addWidget(comboBox_);

  addPage(page);
}


DesignOfExperimentDefinitionItem* DesignOfExperimentSelectionWizard::getSelectedDesignOfExperimentItem()
{
  const int itemRow = comboBox_->currentIndex();
  if (itemRow < 0)
    return 0;
  QVariant variant = comboModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperimentDefinitionItem*>())
    return variant.value<DesignOfExperimentDefinitionItem*>();

  return 0;
}
}