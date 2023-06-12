//                                               -*- C++ -*-
/**
 *  @brief QWizard to select a design of experiments
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
#include "persalys/DesignOfExperimentEvaluationWizard.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/ModelEvaluation.hxx"
#include "persalys/StudyItem.hxx"
#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QDebug>

using namespace OT;

namespace PERSALYS
{

DesignOfExperimentEvaluationWizard::DesignOfExperimentEvaluationWizard(const Analysis& analysis, bool isGeneralWizard, QWidget* parent)
  : Wizard(parent)
  , doesComboBox_(0)
  , doesComboBoxModel_(0)
  , doeLabel_(0)
  , outputsSelectionGroupBox_(0)
  , blockSizeGroupBox_(0)
  , errorMessageLabel_(0)
{
  // check analysis
  DesignOfExperimentEvaluation * currentDoe = dynamic_cast<DesignOfExperimentEvaluation *>(analysis.getImplementation().get());
  Q_ASSERT(currentDoe);

  // build interface
  buildInterface();

  // - fill combo box
  if (isGeneralWizard)
  {
    // get list of DesignOfExperimentDefinitionItem of the current physical model
    if (Observer * obs = analysis.getImplementation().get()->getObserver("Study"))
    {
      StudyImplementation * study = dynamic_cast<StudyImplementation*>(obs);
      Q_ASSERT(study);
      for (UnsignedInteger i = 0; i < study->getAnalyses().getSize(); ++i)
      {
        DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(study->getAnalyses()[i].getImplementation().get());
        ModelEvaluation * modelEval = dynamic_cast<ModelEvaluation *>(study->getAnalyses()[i].getImplementation().get());
        if (doeEval && !modelEval && doeEval->getPhysicalModel() == currentDoe->getPhysicalModel())
        {
          QStandardItem * comboItem = new QStandardItem(QString::fromUtf8(doeEval->getName().c_str()));
          comboItem->setData(QVariant::fromValue(study->getAnalyses()[i]));
          doesComboBoxModel_->appendRow(comboItem);
        }
      }
    }
    connect(doesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWidgets()));
  }
  else
  {
    QStandardItem * comboItem = new QStandardItem(QString::fromUtf8(analysis.getName().c_str()));
    comboItem->setData(QVariant::fromValue(analysis));
    doesComboBoxModel_->appendRow(comboItem);
  }

  // update widgets
  updateWidgets();
}


void DesignOfExperimentEvaluationWizard::buildInterface()
{
  // set window title
  setWindowTitle(tr("Design of experiments evaluation"));
  docLink_ = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#doeevalwizard";

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

  doesComboBox_->setModel(doesComboBoxModel_);
  groupBoxLayout->addWidget(doesComboBox_);

  // doe description
  doeLabel_ = new QLabel;
  groupBoxLayout->addWidget(doeLabel_);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  pageLayout->addWidget(blockSizeGroupBox_);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), errorMessageLabel_, SLOT(reset()));

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  addPage(page);
}


void DesignOfExperimentEvaluationWizard::updateWidgets()
{
  if (!doesComboBox_->count())
    return;
  // get current Analysis
  const DesignOfExperimentEvaluation * analysis_ptr = dynamic_cast<const DesignOfExperimentEvaluation*>(getAnalysis().getImplementation().get());
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



Analysis DesignOfExperimentEvaluationWizard::getAnalysis() const
{
  Q_ASSERT(doesComboBox_);
  Q_ASSERT(doesComboBoxModel_);

  const int itemRow = doesComboBox_->currentIndex();
  if (itemRow < 0)
    return Analysis();

  QVariant variant = doesComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<Analysis>())
    return variant.value<Analysis>();
  return Analysis();
}


bool DesignOfExperimentEvaluationWizard::validateCurrentPage()
{
  if (!doesComboBox_->count())
    return false;

  DesignOfExperimentEvaluation * analysis_ptr = dynamic_cast<DesignOfExperimentEvaluation*>(getAnalysis().getImplementation().get());
  Q_ASSERT(analysis_ptr);

  QString message;
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    message = tr("At least one output must be selected");
  }
  errorMessageLabel_->setErrorMessage(message);
  if (!message.isEmpty())
    return false;

  // update selected DesignOfExperimentEvaluation
  analysis_ptr->setDesignOfExperiment(analysis_ptr->getResult().getDesignOfExperiment()); // reset result
  analysis_ptr->setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis_ptr->setInterestVariables(QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames()));

  return QWizard::validateCurrentPage();
}
}
