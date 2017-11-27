//                                               -*- C++ -*-
/**
 *  @brief QWizard to select a design of experiments
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
#include "otgui/DesignOfExperimentEvaluationWizard.hxx"

#include "otgui/DesignOfExperimentEvaluation.hxx"
#include "otgui/OTStudyItem.hxx"
#include "otgui/QtTools.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QDebug>

using namespace OT;

namespace OTGUI
{

DesignOfExperimentEvaluationWizard::DesignOfExperimentEvaluationWizard(const Analysis& analysis,
    const bool isGeneralWizard,
    QWidget* parent)
  : OTguiWizard(parent)
  , doesComboBox_(0)
  , doesComboBoxModel_(0)
  , doeLabel_(0)
  , outputsSelectionGroupBox_(0)
  , blockSizeGroupBox_(0)
  , errorMessageLabel_(0)
{
  // get list of DesignOfExperimentDefinitionItem of the current physical model
  QList < DesignOfExperimentDefinitionItem* > doeList;
  if (isGeneralWizard)
  {
    PhysicalModel model(dynamic_cast<PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel());
    PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(model.getImplementation().get()->getObserver("PhysicalModelDiagram"));
    Q_ASSERT(pmItem);

    QModelIndexList listIndexes = pmItem->model()->match(pmItem->index(), Qt::UserRole, "DesignsOfExperimentTitle", 1, Qt::MatchRecursive);
    Q_ASSERT(listIndexes.size() == 1);

    QStandardItem * doeTitleItem = pmItem->model()->itemFromIndex(listIndexes[0]);
    Q_ASSERT(doeTitleItem);
    for (int i = 0; i < doeTitleItem->rowCount(); ++i)
    {
      QStandardItem * doeItem = doeTitleItem->child(i);
      Q_ASSERT(doeItem);
      if (doeItem->data(Qt::UserRole).toString().contains("DesignOfExperiment"))
      {
        DesignOfExperimentDefinitionItem * doeDefItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(doeItem);
        Q_ASSERT(doeDefItem);
        if (!doeDefItem->getAnalysis().hasValidResult())
          doeList.append(doeDefItem);
      }
    }
  }
  if (!doeList.size())
  {
    DesignOfExperimentDefinitionItem * doeDefItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(analysis.getImplementation().get()->getObserver("DesignOfExperimentDefinition"));
    Q_ASSERT(doeDefItem);
    doeList.append(doeDefItem);
  }

  // set window title
  setWindowTitle(tr("Design of experiments evaluation"));

  // create a page
  QWizardPage * page = new QWizardPage(this); // create a class QWidget with the doesComboBox_ to reuse it in metamodel wizard
  page->setTitle(tr("Design of experiments evaluation"));
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  // label
  QGroupBox * groupBox = new QGroupBox(tr("Design of experiments"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  pageLayout->addWidget(groupBox);

  // combo box with the list of DesignOfExperimentDefinitionItem
  doesComboBox_ = new QComboBox;
  doesComboBoxModel_ = new QStandardItemModel(doesComboBox_);

  for (int i = 0; i < doeList.count(); ++i)
  {
    QStandardItem * item = new QStandardItem(QString::fromUtf8(doeList[i]->getAnalysis().getName().c_str()));
    item->setData(qVariantFromValue(doeList[i]));
    doesComboBoxModel_->appendRow(item);
  }
  doesComboBox_->setModel(doesComboBoxModel_);
  groupBoxLayout->addWidget(doesComboBox_);
  connect(doesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWidgets()));

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

  // update widgets
  updateWidgets();
}


void DesignOfExperimentEvaluationWizard::updateWidgets()
{
  // get current DesignOfExperimentDefinitionItem
  DesignOfExperimentDefinitionItem * doeItem = getDesignOfExperimentDefinitionItem();
  if (!doeItem)
    return;
  const DesignOfExperimentEvaluation * analysis_ptr = dynamic_cast<const DesignOfExperimentEvaluation*>(doeItem->getAnalysis().getImplementation().get());
  if (!analysis_ptr)
  {
    qDebug() << "DesignOfExperimentEvaluationWizard::updateWidgets: The analysis is not a DesignOfExperimentEvaluation";
    return;
  }

  // update sample size label
  OSS labelTextOss;
  labelTextOss << tr("Input sample size = ").toStdString() << analysis_ptr->getOriginalInputSample().getSize();
  doeLabel_->setText(labelTextOss.str().c_str());

  // update interest variables list
  outputsSelectionGroupBox_->updateComboBoxModel(analysis_ptr->getPhysicalModel().getSelectedOutputsNames(),
      analysis_ptr->getInterestVariables());
  // update block size
  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());
}


DesignOfExperimentDefinitionItem * DesignOfExperimentEvaluationWizard::getDesignOfExperimentDefinitionItem() const
{
  Q_ASSERT(doesComboBox_);
  Q_ASSERT(doesComboBoxModel_);

  const int itemRow = doesComboBox_->currentIndex();
  if (itemRow < 0)
    return 0;

  QVariant variant = doesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperimentDefinitionItem*>())
    return variant.value<DesignOfExperimentDefinitionItem*>();
  return 0;
}


void DesignOfExperimentEvaluationWizard::clearErrorMessage()
{
  // clear() method does not work...
  errorMessageLabel_->setText("");
}


bool DesignOfExperimentEvaluationWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");

  DesignOfExperimentDefinitionItem * doeItem = getDesignOfExperimentDefinitionItem();
  Q_ASSERT(doeItem);

  QString message;
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    message = tr("At least one output must be selected");
  }
  if (blockSizeGroupBox_->getBlockSizeValue() > getDesignOfExperimentDefinitionItem()->getOriginalInputSample().getSize())
  {
    message = tr("The block size must be inferior or equal to the size of the input sample");
  }
  if (!message.isEmpty())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(message));
    return false;
  }

  // update selected DesignOfExperimentEvaluation
  DesignOfExperimentEvaluation * analysis_ptr = dynamic_cast<DesignOfExperimentEvaluation*>(doeItem->getAnalysis().getImplementation().get());
  Q_ASSERT(analysis_ptr);

  analysis_ptr->setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis_ptr->setInterestVariables(QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames()));

  return QWizard::validateCurrentPage();
}
}
