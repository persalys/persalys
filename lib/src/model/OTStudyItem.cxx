//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a otStudy
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
#include "otgui/OTStudyItem.hxx"

#include "otgui/ReliabilityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

OTStudyItem::OTStudyItem(OTStudy * otStudy)
  : QObject()
  , QStandardItem(QString::fromUtf8(otStudy->getName().c_str()))
  , Observer("OTStudy")
  , otStudy_(otStudy)
{
  otStudy_->addObserver(this);
  setData("OTStudy", Qt::UserRole);
  setToolTip(QString::fromUtf8(otStudy_->getFileName().c_str()));
}


void OTStudyItem::update(Observable * source, const String & message)
{
  if (message == "addDataModel")
  {
    DataModel addedDataModel = otStudy_->getDataModels()[otStudy_->getDataModels().getSize()-1];
    addDataModelItem(addedDataModel);
  }
  else if (message == "addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = otStudy_->getPhysicalModels()[otStudy_->getPhysicalModels().getSize()-1];
    addPhysicalModelItem(addedPhysicalModel);
  }
  else if (message == "addDesignOfExperiment")
  {
    DesignOfExperiment addedDesignOfExperiment = otStudy_->getDesignOfExperiments()[otStudy_->getDesignOfExperiments().getSize()-1];
    try
    {
      addDesignOfExperimentItem(addedDesignOfExperiment);
    }
    catch (std::exception & ex)
    {
      std::cerr<<"No item added for the design of experiment named " << addedDesignOfExperiment.getName() << "\n" << ex.what() << std::endl;
    }
  }
  else if (message == "addLimitState")
  {
    LimitState addedLimitState = otStudy_->getLimitStates()[otStudy_->getLimitStates().getSize()-1];
    try
    {
      addLimitStateItem(addedLimitState);
    }
    catch (std::exception & ex)
    {
      std::cerr<<"No item added for the limit state named " << addedLimitState.getName() << "\n" << ex.what() << std::endl;
    }
  }
  else if (message == "addAnalysis")
  {
    Analysis addedAnalysis = otStudy_->getAnalyses()[otStudy_->getAnalyses().getSize()-1];
    try
    {
      addAnalysisItem(addedAnalysis);
    }
    catch (std::exception & ex)
    {
      std::cerr<<"No item added for the analysis named " << addedAnalysis.getName() << "\n" << ex.what() << std::endl;
    }
  }
  else if (message == "otStudyRemoved")
  {
    emit otStudyRemoved(this);
  }
  else if (message == "fileNameChanged")
  {
    setToolTip(QString::fromUtf8(otStudy_->getFileName().c_str()));
  }
  else
  {
    std::cerr<< "In OTStudyItem::update: not recognized message: " << message << std::endl;
  }
}


void OTStudyItem::updateAnalysis(const Analysis & analysis)
{
  otStudy_->getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());
}


void OTStudyItem::updateDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  otStudy_->getDesignOfExperimentByName(designOfExperiment.getName()).setImplementationAsPersistentObject(designOfExperiment.getImplementation());
}


void OTStudyItem::addDataModelItem(DataModel & dataModel)
{
  // Physical model item
  DataModelItem * newDataModelItem = new DataModelItem(dataModel);
  connect(newDataModelItem, SIGNAL(dataModelRemoved(QStandardItem*)), this, SLOT(removeItem(QStandardItem*)));
  appendRow(newDataModelItem);

  emit newDataModelItemCreated(newDataModelItem);
}


void OTStudyItem::addPhysicalModelItem(PhysicalModel & physicalModel)
{
  // Physical model item
  PhysicalModelItem * newPhysicalModelItem = new PhysicalModelItem(physicalModel);
  connect(newPhysicalModelItem, SIGNAL(physicalModelRemoved(QStandardItem*)), this, SLOT(removeItem(QStandardItem*)));
  appendRow(newPhysicalModelItem);

  // Deterministic study item
  QStandardItem * item = new QStandardItem(tr("Deterministic study"));
  item->setData("DeterministicStudy", Qt::UserRole);
  item->setEditable(false);
  newPhysicalModelItem->appendRow(item);

  // Probabilistic study item
  item = new QStandardItem(tr("Probabilistic study"));
  item->setData("ProbabilisticStudy", Qt::UserRole);
  item->setEditable(false);
  newPhysicalModelItem->appendRow(item);

  // Probabilistic model item
  if (physicalModel.hasStochasticInputs())
  {
    ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(physicalModel);
    item->appendRow(newProbabilisticModelItem);
    connect(newProbabilisticModelItem, SIGNAL(probabilisticModelRemoved(QStandardItem*)), this, SLOT(removeItem(QStandardItem*)));
    emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
  }

  // Designs of experiment item
  item = new QStandardItem(tr("Designs of experiment"));
  item->setData("DesignOfExperimentList", Qt::UserRole);
  item->setEditable(false);
  newPhysicalModelItem->appendRow(item);

  emit newPhysicalModelItemCreated(newPhysicalModelItem);
}


void OTStudyItem::addDesignOfExperimentItem(DesignOfExperiment & design)
{
  DesignOfExperimentItem * newItem = new DesignOfExperimentItem(design);

  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == design.getPhysicalModel().getName())
    {
      child(i)->child(2)->appendRow(newItem);
      emit newDesignOfExperimentItemCreated(newItem);
      connect(newItem, SIGNAL(designOfExperimentRemoved(QStandardItem*)), this, SLOT(removeItem(QStandardItem*)));
      connect(newItem, SIGNAL(designOfExperimentChanged(DesignOfExperiment)), this, SLOT(updateDesignOfExperiment(DesignOfExperiment)));
      return;
    }
  throw InvalidArgumentException(HERE) << "No physical model matches the given name " << design.getPhysicalModel().getName();
}


void OTStudyItem::addLimitStateItem(LimitState & limitState)
{
  LimitStateItem * newItem = new LimitStateItem(limitState);

  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == limitState.getPhysicalModel().getName())
    {
      if (!limitState.getPhysicalModel().hasStochasticInputs() && !child(i)->child(1)->hasChildren())
      {
        ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(limitState.getPhysicalModel());
        limitState.getPhysicalModel().addObserver(newProbabilisticModelItem);
        child(i)->child(1)->appendRow(newProbabilisticModelItem);
        emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
      }
      limitState.getPhysicalModel().addObserver(newItem);
      child(i)->child(1)->appendRow(newItem);
      emit newLimitStateItemCreated(newItem);
      connect(newItem, SIGNAL(limitStateRemoved(QStandardItem*)), this, SLOT(removeItem(QStandardItem*)));
      return;
    }
  throw InvalidArgumentException(HERE) << "No physical model matches the name " << limitState.getPhysicalModel().getName();
}


void OTStudyItem::addAnalysisItem(Analysis & analysis)
{
  String analysisName = analysis.getImplementation()->getClassName();
  AnalysisItem * newItem = new AnalysisItem(analysis, analysisName);
  if (analysisName == "ModelEvaluation")
  {
    addDeterministicAnalysisItem(analysis, newItem);
  }
  else if (analysisName == "MonteCarloAnalysis" || analysisName == "TaylorExpansionMomentsAnalysis" ||
           analysisName == "SobolAnalysis"      || analysisName == "SRCAnalysis")
  {
    addProbabilisticAnalysisItem(analysis, newItem);
  }
  else if (analysisName == "MonteCarloReliabilityAnalysis")
  {
    addReliabilityAnalysisItem(analysis, newItem);
  }
  else if (analysisName == "DataAnalysis")
  {
    addDataModelAnalysisItem(analysis, newItem);
  }
  else
  {
    throw InvalidArgumentException(HERE) << "In OTStudyItem::addAnalysisItem: Impossible to add an item for the analysis of type " << analysisName;
  }
  connect(newItem, SIGNAL(analysisChanged(Analysis)), this, SLOT(updateAnalysis(Analysis)));
  connect(newItem, SIGNAL(analysisRemoved(QStandardItem*)), this, SLOT(removeItem(QStandardItem*)));
}


void OTStudyItem::addDataModelAnalysisItem(Analysis& analysis, AnalysisItem* item)
{
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getModelName())
    {
      child(i)->appendRow(item);
      emit newAnalysisItemCreated(item);
      return;
    }
  std::cerr<<"No item added for the data analysis named "<<analysis.getName()<<std::endl;

}


void OTStudyItem::addDeterministicAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getModelName())
    {
      child(i)->child(0)->appendRow(item);
      emit newAnalysisItemCreated(item);
      return;
    }
  std::cerr<<"No item added for the deterministic analysis named "<<analysis.getName()<<std::endl;
}


void OTStudyItem::addProbabilisticAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getModelName())
    {
      if (!child(i)->child(1)->hasChildren())
      {
        PhysicalModel physicalModel(dynamic_cast<PhysicalModelAnalysis*>(&*analysis.getImplementation())->getPhysicalModel());
        ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(physicalModel);
        physicalModel.addObserver(newProbabilisticModelItem);
        child(i)->child(1)->appendRow(newProbabilisticModelItem);
        emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
      }
      child(i)->child(1)->appendRow(item);
      emit newAnalysisItemCreated(item);
      return;
    }
  std::cerr<<"No item added for the probabilistic analysis named "<<analysis.getName()<<std::endl;
}


void OTStudyItem::addReliabilityAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  String limitStateName = dynamic_cast<ReliabilityAnalysis*>(&*analysis.getImplementation())->getLimitState().getName();
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getModelName())
    {
      QStandardItem * probabilisticStudyItem = child(i)->child(1);
      for (int j=0; j<probabilisticStudyItem->rowCount(); ++j)
        if (probabilisticStudyItem->child(j)->text().toStdString() == limitStateName)
        {
          probabilisticStudyItem->child(j)->appendRow(item);
          emit newAnalysisItemCreated(item);
          return;
        }
    }
  std::cerr<<"No item added for the reliability analysis named "<<analysis.getName()<<std::endl;
}


void OTStudyItem::removeItem(QStandardItem * item)
{
  emit itemRemoved(item);
  item->QStandardItem::parent()->removeRow(item->row());
}


void OTStudyItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    otStudy_->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


OTStudy* OTStudyItem::getOTStudy()
{
  return otStudy_;
}
}