//                                               -*- C++ -*-
/**
 *  @brief Model of the study tree view, observer of the studies
 *
 *  Copyright 2015-2016 EDF
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

StudyTreeViewModel::StudyTreeViewModel()
  : QStandardItemModel()
  , Observer()
{
}


StudyTreeViewModel::~StudyTreeViewModel()
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
  OTStudy * newStudy = new OTStudy(OTStudy::GetAvailableOTStudyName());
  OTStudy::AddOTStudy(newStudy);
}


void StudyTreeViewModel::addOTStudyItem(OTStudy * otStudy)
{
  OTStudyItem * otStudyItem = new OTStudyItem(otStudy);
  connect(otStudyItem, SIGNAL(newPhysicalModelItemCreated(PhysicalModelItem*)), this, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)));
  connect(otStudyItem, SIGNAL(newProbabilisticModelItemCreated(ProbabilisticModelItem*)), this, SIGNAL(newProbabilisticModelCreated(ProbabilisticModelItem*)));
  connect(otStudyItem, SIGNAL(newDesignOfExperimentItemCreated(DesignOfExperimentItem*)), this, SIGNAL(newDesignOfExperimentCreated(DesignOfExperimentItem*)));
  connect(otStudyItem, SIGNAL(newLimitStateItemCreated(LimitStateItem*)), this, SIGNAL(newLimitStateCreated(LimitStateItem*)));
  connect(otStudyItem, SIGNAL(newAnalysisItemCreated(AnalysisItem*)), this, SIGNAL(newAnalysisCreated(AnalysisItem*)));
  otStudy->addObserver(otStudyItem);
  invisibleRootItem()->appendRow(otStudyItem);
  for (UnsignedInteger i=0; i<otStudy->getPhysicalModels().getSize(); ++i)
    otStudyItem->addPhysicalModelItem(otStudy->getPhysicalModels()[i]);
  for (UnsignedInteger i=0; i<otStudy->getDesignOfExperiments().getSize(); ++i)
    otStudyItem->addDesignOfExperimentItem(otStudy->getDesignOfExperiments()[i]);
  for (UnsignedInteger i=0; i<otStudy->getLimitStates().getSize(); ++i)
    otStudyItem->addLimitStateItem(otStudy->getLimitStates()[i]);
  for (UnsignedInteger i=0; i<otStudy->getAnalyses().getSize(); ++i)
  {
    otStudyItem->addAnalysisItem(otStudy->getAnalyses()[i]);
    if (otStudy->getAnalyses()[i].analysisLaunched())
      otStudy->getAnalyses()[i].getImplementation()->notify("analysisFinished");
  }
}


void StudyTreeViewModel::addProbabilisticModelItem(const QModelIndex & parentIndex)
{
  PhysicalModelItem * parentItem = static_cast<PhysicalModelItem*>(itemFromIndex(parentIndex)->parent());
  ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(parentItem->getPhysicalModel());
  parentItem->getPhysicalModel().addObserver(newProbabilisticModelItem);
  connect(parentItem, SIGNAL(physicalModelChanged(PhysicalModel)), newProbabilisticModelItem, SLOT(updatePhysicalModel(PhysicalModel)));
  itemFromIndex(parentIndex)->appendRow(newProbabilisticModelItem);
  emit newProbabilisticModelCreated(newProbabilisticModelItem);
}


void StudyTreeViewModel::addLimitStateItem(const QModelIndex & parentIndex)
{
  PhysicalModelItem * parentItem = static_cast<PhysicalModelItem*>(itemFromIndex(parentIndex)->parent());
  PhysicalModel physicalModel = parentItem->getPhysicalModel();
  OTStudyItem * studyItem = getOTStudyItem(parentIndex);
  LimitState newLimitState(studyItem->getOTStudy()->getAvailableLimitStateName(), physicalModel, physicalModel.getOutputs()[0].getName(), OT::Less(), 0.);
  studyItem->getOTStudy()->addLimitState(newLimitState);
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
}