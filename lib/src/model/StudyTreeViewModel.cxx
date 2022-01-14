//                                               -*- C++ -*-
/**
 *  @brief Model of the study tree view, observer of the studies
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

const int StudyTreeViewModel::DefaultSignificantDigits = 12;

StudyTreeViewModel::StudyTreeViewModel(QObject *parent)
  : QStandardItemModel(parent)
  , Observer()
{
  setColumnCount(1);
}


QVariant StudyTreeViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (section == 0 && orientation == Qt::Horizontal)
  {
    if (role == Qt::DisplayRole)
      return tr("Studies");
  }

  return QStandardItemModel::headerData(section, orientation, role);
}


void StudyTreeViewModel::update(Observable * /*source*/, const String & message)
{
  if (message == "addStudy")
  {
    appendItem(Study::GetInstances()[Study::GetInstances().getSize() - 1]);
  }
}


void StudyTreeViewModel::appendItem(const Study & study)
{
  StudyItem * studyItem = new StudyItem(study);

  // append the item
  invisibleRootItem()->appendRow(studyItem);

  // signal for StudyTreeView to create the window
  emit windowRequested(studyItem);

  // add sub items
  for (UnsignedInteger i = 0; i < study.getPhysicalModels().getSize(); ++i)
  {
    studyItem->appendItem(study.getPhysicalModels()[i]);
    study.getPhysicalModels()[i].addObserver(study.getImplementation().get());
  }

  for (UnsignedInteger i = 0; i < study.getDataModels().getSize(); ++i)
  {
    studyItem->appendItem(study.getDataModels()[i]);
    study.getDataModels()[i].addObserver(study.getImplementation().get());
  }

  for (UnsignedInteger i = 0; i < study.getLimitStates().getSize(); ++i)
  {
    studyItem->appendItem(study.getLimitStates()[i]);
    study.getLimitStates()[i].addObserver(study.getImplementation().get());
  }

  for (UnsignedInteger i = 0; i < study.getAnalyses().getSize(); ++i)
  {
    studyItem->appendItem(study.getAnalyses()[i]);
    study.getAnalyses()[i].addObserver(study.getImplementation().get());
  }

  // signal for StudyTreeView to collapse models items
  emit studySubItemsAdded(studyItem);
}
}
