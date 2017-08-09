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

#include "otgui/DesignOfExperimentAnalysis.hxx"
#include "otgui/OTStudyItem.hxx"
#include "otgui/QtTools.hxx"

#include <QVBoxLayout>
#include <QGroupBox>

using namespace OT;

namespace OTGUI {

DesignOfExperimentSelectionWizard::DesignOfExperimentSelectionWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard, QWidget* parent)
  : OTguiWizard(parent)
  , doesComboBox_(0)
  , doesComboBoxModel_(0)
  , doeLabel_(0)
  , outputsSelectionGroupBox_(0)
  , blockSizeGroupBox_(0)
  , errorMessageLabel_(0)
{
  // get list of DesignOfExperiments of the current physical model
  const DesignOfExperiment doe = dynamic_cast<DesignOfExperimentAnalysis*>(analysis.getImplementation().get())->getDesignOfExperiment();
  QList < DesignOfExperiment > doeList;
  if (isGeneralWizard)
  {
    for (UnsignedInteger i = 0; i < item->getParentOTStudyItem()->getOTStudy().getDesignOfExperiments().getSize(); ++i)
    {
      const DesignOfExperiment doe_i = item->getParentOTStudyItem()->getOTStudy().getDesignOfExperiments()[i];
      if (doe_i.getPhysicalModel().getImplementation().get() == doe.getPhysicalModel().getImplementation().get() &&
         !doe_i.getOutputSample().getSize())
        doeList.append(doe_i);
    }
  }
  else
  {
    doeList.append(doe);
  }

  // set window title
  setWindowTitle(tr("Design of experiment evaluation"));

  // create a page
  QWizardPage * page = new QWizardPage(this); // create a class QWidget with the doesComboBox_ to reuse it in metamodel wizard
  page->setTitle(tr("Design of experiment evaluation"));
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  // label
  QGroupBox * groupBox = new QGroupBox(tr("Design of experiment"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  pageLayout->addWidget(groupBox);

  // combo box with the list of DesignOfExperiments
  doesComboBox_ = new QComboBox;
  doesComboBoxModel_ = new QStandardItemModel(doesComboBox_);

  for (int i = 0; i < doeList.count(); ++i)
  {
    QStandardItem * item = new QStandardItem(doeList[i].getName().c_str());
    item->setData(qVariantFromValue(doeList[i]));
    doesComboBoxModel_->appendRow(item);
  }
  doesComboBox_->setModel(doesComboBoxModel_);
  groupBoxLayout->addWidget(doesComboBox_);
  connect(doesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWidgets(int)));

  // doe description
  doeLabel_ = new QLabel;
  groupBoxLayout->addWidget(doeLabel_);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(clearErrorMessage()));
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), this, SLOT(clearErrorMessage()));
  pageLayout->addWidget(blockSizeGroupBox_);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  addPage(page);
  updateWidgets(0);
}


DesignOfExperiment DesignOfExperimentSelectionWizard::getDesignOfExperiment() const
{
  const int itemRow = doesComboBox_->currentIndex();
  if (itemRow < 0)
    return 0;

  const QVariant variant = doesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperiment>())
  {
    DesignOfExperiment doe = variant.value<DesignOfExperiment>();
    doe.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
    doe.setInterestVariables(QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames()));
    return doe;
  }
  return DesignOfExperiment();
}


void DesignOfExperimentSelectionWizard::updateWidgets(int index)
{
  if (index < 0)
    return;

  const QVariant variant = doesComboBoxModel_->item(index)->data();
  if (variant.canConvert<DesignOfExperiment>())
  {
    const DesignOfExperiment doe = variant.value<DesignOfExperiment>();
    OSS labelTextOss;
    labelTextOss << tr("Input sample size = ").toStdString() << doe.getOriginalInputSample().getSize();
    doeLabel_->setText(labelTextOss.str().c_str());
  }

  // get current DesignOfExperimentDefinitionItem
  DesignOfExperiment currentDoe = getCurrentDesignOfExperiment();
  // update block size
  outputsSelectionGroupBox_->updateComboBoxModel(currentDoe.getPhysicalModel().getSelectedOutputsNames(),
                                                 currentDoe.getInterestVariables());
  // update block size
  blockSizeGroupBox_->setBlockSizeValue(currentDoe.getBlockSize());
}


DesignOfExperiment DesignOfExperimentSelectionWizard::getCurrentDesignOfExperiment() const
{
  const int itemRow = doesComboBox_->currentIndex();
  if (itemRow < 0)
    return 0;

  QVariant variant = doesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperiment>())
    return variant.value<DesignOfExperiment>();
  return DesignOfExperiment();
}


void DesignOfExperimentSelectionWizard::clearErrorMessage()
{
  // clear() method does not work...
  errorMessageLabel_->setText("");
}


bool DesignOfExperimentSelectionWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");

  QString message;
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    message = tr("At least one output must be selected");
  }
  if (blockSizeGroupBox_->getBlockSizeValue() > getCurrentDesignOfExperiment().getOriginalInputSample().getSize())
  {
    message = tr("The block size must be inferior or equal to the size of the input sample");
  }
  if (!message.isEmpty())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(message));
    return false;
  }

  return QWizard::validateCurrentPage();
}
}
