//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
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
#include "persalys/DataSensitivityAnalysis.hxx"
#include "persalys/CopulaInferenceAnalysis.hxx"
#include "persalys/MetaModelAnalysis.hxx"
#include "persalys/DataAnalysis.hxx"

#include "persalys/DataModelDiagramItem.hxx"
#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

DataModelDiagramItem::DataModelDiagramItem(const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DataModelDiagramItem")
{
  buildActions();
}


void DataModelDiagramItem::buildActions()
{
  // define data model action
  defineAction_ = new QAction(tr("Define the model"), this);
  defineAction_->setStatusTip(tr("Define the data model"));
  connect(defineAction_, SIGNAL(triggered(bool)), this, SLOT(appendDataModelItem()));

  // new analyses
  newDataAnalysis_ = createAction("DataAnalysis", getDesignOfExperiment());
  newQuantileAnalysis_ = createAction("QuantileAnalysis", getDesignOfExperiment());
  newInferenceAnalysis_ = createAction("Inference", getDesignOfExperiment());
  newCopulaInferenceAnalysis_ = createAction("CopulaInference", getDesignOfExperiment());
  newMetaModel_ = createAction("Metamodel", getDesignOfExperiment());
  newDataSensitivityAnalysis_ = createAction("DataSensitivityAnalysis", getDesignOfExperiment());

  // remove data model
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the data model"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // add actions
  appendAction(defineAction_);
  appendSeparator();
  appendAction(removeAction_);
}


void DataModelDiagramItem::update(Observable* /*source*/, const String& message)
{
  if (message == "variablesChanged")
  {
    if (!hasChildren()) // if modification from Python console
    {
      fill();
      return;
    }
    // emit signals to DataModelDiagramWindow
    // to update the diagram (arrow color and button availability)
    String errorMessage;
    bool validity;
    // CanBeLaunched need to be called before emit otherwise errorMessage might be sent before being modified
    validity = DataAnalysis::CanBeLaunched(errorMessage, designOfExperiment_); 
    emit dataSizeValidityChanged(validity, QString(errorMessage.c_str()));
    validity = DesignOfExperimentAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
    emit dataModelValidityChanged(validity, QString(errorMessage.c_str()));
    validity = DataSensitivityAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
    emit dataSensitivityValidityChanged(validity, QString(errorMessage.c_str()));
    validity = CopulaInferenceAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
    emit dependenciesValidityChanged(validity, QString(errorMessage.c_str()));
    validity = MetaModelAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
    emit metaModelValidityChanged(validity, QString(errorMessage.c_str()));
    validity = DesignOfExperimentAnalysis::CanBeLaunched(errorMessage, designOfExperiment_) && metamodelCounter_ > 0;
    QString mmNumberMessage = errorMessage.empty() ? tr("Metamodel must first be created") : QString{errorMessage.c_str()};
    emit metamodelNumberValidityChanged(validity, mmNumberMessage);
  }
  else if (message == "analysisLaunched")
  {
    analysisInProgress_ = true;
  }
  else if (message == "analysisFinished" || message == "analysisBadlyFinished")
  {
    analysisInProgress_ = false;
  }
  else if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "DataModelDiagramItem::update(objectRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


void DataModelDiagramItem::fill()
{
  // model definition item
  if (designOfExperiment_.getSample().getSize())
    appendDataModelItem();

  // update diagram (arrow color and button availability)
  String errorMessage;
  bool validity;
  validity = DataAnalysis::CanBeLaunched(errorMessage, designOfExperiment_); 
  emit dataSizeValidityChanged(validity, QString(errorMessage.c_str()));
  validity = DesignOfExperimentAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
  emit dataModelValidityChanged(validity, QString(errorMessage.c_str()));
  validity = DataSensitivityAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
  emit dataSensitivityValidityChanged(validity, QString(errorMessage.c_str()));
  validity = CopulaInferenceAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
  emit dependenciesValidityChanged(validity, QString(errorMessage.c_str()));
  validity = MetaModelAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
  emit metaModelValidityChanged(validity, QString(errorMessage.c_str()));
  validity = DesignOfExperimentAnalysis::CanBeLaunched(errorMessage, designOfExperiment_) && metamodelCounter_ > 0;
  QString mmNumberMessage = errorMessage.empty() ? tr("Metamodel must first be created") : QString{errorMessage.c_str()};
  emit metamodelNumberValidityChanged(validity, mmNumberMessage);
}


void DataModelDiagramItem::appendDataModelItem()
{
  if (hasChildren())
  {
    emit changeCurrentItemRequested(child(0)->index());
    return;
  }

  // new item
  DataModelDefinitionItem * dmItem = new DataModelDefinitionItem(getDesignOfExperiment(), this);
  appendRow(dmItem);

  // emit signal to the StudyTreeView to create a window
  emit windowRequested(dmItem);

  // disable the definition action
  defineAction_->setDisabled(true);
}


void DataModelDiagramItem::appendItem(const Analysis& analysis)
{
  appendAnalysisItem(analysis);
  analysis.getImplementation().get()->addObserver(this);
  analysis.getImplementation().get()->addObserver(getParentStudyItem());

  const QString analysisName {analysis.getImplementation()->getClassName().c_str()};
  if (analysisName == "FunctionalChaosAnalysis" ||
    analysisName == "KrigingAnalysis" ||
    analysisName == "PolynomialRegressionAnalysis")
  {
    const auto * newItem = dynamic_cast<AnalysisItem*>(analysis.getImplementation().get()->getObserver("AnalysisItem"));
    connect(newItem, &AnalysisItem::numberMetamodelChanged, this, &DataModelDiagramItem::updateMetamodelCounter);

    if(newItem->getAnalysis().hasValidResult())
      ++metamodelCounter_;

    OT::String errorMessage; 
    emit metamodelNumberValidityChanged(
      MetaModelAnalysis::CanBeLaunched(errorMessage, designOfExperiment_) && metamodelCounter_ > 0,
      errorMessage.empty() ? tr("Metamodel must first be created") : QString{errorMessage.c_str()}
    );
  }
}


void DataModelDiagramItem::removeDesignOfExperiment()
{
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a design of experiments when an analysis is running."));
    return;
  }
  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(DesignOfExperiment(designOfExperiment_));
}

void DataModelDiagramItem::updateMetamodelCounter(int increment)
{
  metamodelCounter_ += increment;
  if (metamodelCounter_ < 0)
    metamodelCounter_ = 0;

  OT::String errorMessage;
  emit metamodelNumberValidityChanged(
    MetaModelAnalysis::CanBeLaunched(errorMessage, designOfExperiment_) && metamodelCounter_ > 0,
    errorMessage.empty() ? tr("Metamodel must first be created") : QString{errorMessage.c_str()}
  );
}

void DataModelDiagramItem::requestMetaModelExport()
{
  const Study study{getParentStudyItem()->getStudy()};
  // we can pass any analysis as it is just used by the wizard to get the Study observer
  // declared in ItemFactory, connected to StudyManager
  emit mmExportWizardRequested(getParentStudyItem(), study.getAnalyses()[0], true);
}

} // namespace PERSALYS
