//                                               -*- C++ -*-
/**
 *  @brief Model of the study tree view, observer of the studies
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
#include "otgui/StudyTreeViewModel.hxx"

#include "otgui/OTStudy.hxx"
#include "otgui/AnalyticalPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

StudyTreeViewModel::StudyTreeViewModel(QObject *parent)
  : QStandardItemModel(parent)
  , Observer()
{
}


void StudyTreeViewModel::update(Observable * source, const String & message)
{
  if (message == "addStudy")
  {
    OTStudy * otStudy = static_cast<OTStudy*>(source);
    addOTStudyItem(otStudy);
  }
}


void StudyTreeViewModel::createNewOTStudy()
{
  OTStudy::Add(new OTStudy(OTStudy::GetAvailableName()));
}


void StudyTreeViewModel::addOTStudyItem(OTStudy * otStudy)
{
  OTStudyItem * otStudyItem = new OTStudyItem(otStudy);
  connect(otStudyItem, SIGNAL(newDataModelItemCreated(DesignOfExperimentItem*)), this, SIGNAL(newDataModelCreated(DesignOfExperimentItem*)));
  connect(otStudyItem, SIGNAL(newPhysicalModelItemCreated(PhysicalModelItem*)), this, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)));
  connect(otStudyItem, SIGNAL(newProbabilisticModelItemCreated(ProbabilisticModelItem*)), this, SIGNAL(newProbabilisticModelCreated(ProbabilisticModelItem*)));
  connect(otStudyItem, SIGNAL(newDesignOfExperimentItemCreated(DesignOfExperimentItem*)), this, SIGNAL(newDesignOfExperimentCreated(DesignOfExperimentItem*)));
  connect(otStudyItem, SIGNAL(newLimitStateItemCreated(LimitStateItem*)), this, SIGNAL(newLimitStateCreated(LimitStateItem*)));
  connect(otStudyItem, SIGNAL(newAnalysisItemCreated(AnalysisItem*)), this, SIGNAL(newAnalysisCreated(AnalysisItem*)));
  connect(otStudyItem, SIGNAL(itemRemoved(QStandardItem*)), this, SIGNAL(itemRemoved(QStandardItem*)));
  connect(otStudyItem, SIGNAL(otStudyRemoved(QStandardItem*)), this, SLOT(removeOTStudyItem(QStandardItem*)));

  invisibleRootItem()->appendRow(otStudyItem);
  for (UnsignedInteger i=0; i<otStudy->getDataModels().getSize(); ++i)
    otStudyItem->addDataModelItem(otStudy->getDataModels()[i]);
  for (UnsignedInteger i=0; i<otStudy->getPhysicalModels().getSize(); ++i)
    otStudyItem->addPhysicalModelItem(otStudy->getPhysicalModels()[i]);
  for (UnsignedInteger i=0; i<otStudy->getLimitStates().getSize(); ++i)
    otStudyItem->addLimitStateItem(otStudy->getLimitStates()[i]);
  for (UnsignedInteger i=0; i<otStudy->getAnalyses().getSize(); ++i)
  {
    otStudyItem->addAnalysisItem(otStudy->getAnalyses()[i]);
    if (otStudy->getAnalyses()[i].analysisLaunched())
      otStudy->getAnalyses()[i].getImplementation()->notify("analysisFinished");
  }
  for (UnsignedInteger i=0; i<otStudy->getDesignOfExperiments().getSize(); ++i)
    otStudyItem->addDesignOfExperimentItem(otStudy->getDesignOfExperiments()[i]);
}


void StudyTreeViewModel::addProbabilisticModelItem(const QModelIndex & parentIndex)
{
  PhysicalModelItem * parentItem = static_cast<PhysicalModelItem*>(itemFromIndex(parentIndex)->parent());
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(parentItem->QStandardItem::parent());
  ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(parentItem->getPhysicalModel());
  parentItem->getPhysicalModel().addObserver(newProbabilisticModelItem);
  connect(newProbabilisticModelItem, SIGNAL(probabilisticModelRemoved(QStandardItem*)), studyItem, SLOT(removeItem(QStandardItem*)));
  itemFromIndex(parentIndex)->appendRow(newProbabilisticModelItem);
  emit newProbabilisticModelCreated(newProbabilisticModelItem);
}


void StudyTreeViewModel::addLimitStateItem(const QModelIndex & parentIndex)
{
  PhysicalModelItem * parentItem = static_cast<PhysicalModelItem*>(itemFromIndex(parentIndex)->parent());
  PhysicalModel physicalModel = parentItem->getPhysicalModel();
  OTStudyItem * studyItem = getOTStudyItem(parentIndex);
  LimitState newLimitState(studyItem->getOTStudy()->getAvailableLimitStateName(), physicalModel, physicalModel.getOutputs()[0].getName(), OT::Less(), 0.);
  studyItem->getOTStudy()->add(newLimitState);
}


OTStudyItem* StudyTreeViewModel::getOTStudyItem(const QModelIndex & childIndex)
{
  QModelIndex seekRoot = childIndex;
  while(seekRoot.parent() != QModelIndex())
    seekRoot = seekRoot.parent();

  return static_cast<OTStudyItem*>(itemFromIndex(seekRoot));
}


PhysicalModelItem* StudyTreeViewModel::getPhysicalModelItem(const QModelIndex & childIndex)
{
  QModelIndex seekRoot = childIndex;
  while(seekRoot.parent() != QModelIndex())
  {
    seekRoot = seekRoot.parent();
    if (itemFromIndex(seekRoot)->data(Qt::UserRole).toString() == "PhysicalModel")
      return static_cast<PhysicalModelItem*>(itemFromIndex(seekRoot));
  }
  return 0;
}


LimitStateItem* StudyTreeViewModel::getLimitStateItem(const QModelIndex & childIndex)
{
  if (itemFromIndex(childIndex)->data(Qt::UserRole).toString() == "LimitState")
    return static_cast<LimitStateItem*>(itemFromIndex(childIndex));
  QModelIndex seekRoot = childIndex;
  while(seekRoot.parent() != QModelIndex())
  {
    seekRoot = seekRoot.parent();
    if (itemFromIndex(seekRoot)->data(Qt::UserRole).toString() == "LimitState")
      return static_cast<LimitStateItem*>(itemFromIndex(seekRoot));
  }
  return 0;
}


AnalysisItem* StudyTreeViewModel::getAnalysisItem(OTStudyItem * otStudyItem, const QString & analysisName)
{
  QModelIndexList listIndexes = match(otStudyItem->index(), Qt::DisplayRole, analysisName, 2, Qt::MatchRecursive);
  if (listIndexes.size() == 1)
    return dynamic_cast<AnalysisItem*>(itemFromIndex(listIndexes[0]));
  return 0;
}


void StudyTreeViewModel::removeOTStudyItem(QStandardItem * item)
{
  invisibleRootItem()->removeRow(item->row());
}
}
