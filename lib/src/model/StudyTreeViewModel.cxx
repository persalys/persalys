//                                               -*- C++ -*-
/**
 *  @brief Model of the study tree view, observer of the studies
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/StudyTreeViewModel.hxx"

using namespace OT;

namespace OTGUI
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

    else if (role == Qt::DecorationRole)
      return QIcon(":/images/OT_icon16x16.png");
  }

  return QStandardItemModel::headerData(section, orientation, role);
}


void StudyTreeViewModel::update(Observable * source, const String & message)
{
  if (message == "addStudy")
  {
    appendItem(OTStudy::GetInstances()[OTStudy::GetInstances().getSize() - 1]);
  }
}


void StudyTreeViewModel::appendItem(const OTStudy & otStudy)
{
  OTStudyItem * otStudyItem = new OTStudyItem(otStudy);

  // append the item
  invisibleRootItem()->appendRow(otStudyItem);

  // signal for StudyTreeView to create the window
  emit studyCreated(otStudyItem);

  // add sub items
  for (UnsignedInteger i = 0; i < otStudy.getDataModels().getSize(); ++i)
  {
    otStudyItem->appendItem(otStudy.getDataModels()[i]);
    otStudy.getDataModels()[i].addObserver(otStudy.getImplementation().get());
  }

  for (UnsignedInteger i = 0; i < otStudy.getPhysicalModels().getSize(); ++i)
  {
    otStudyItem->appendItem(otStudy.getPhysicalModels()[i]);
    otStudy.getPhysicalModels()[i].addObserver(otStudy.getImplementation().get());
  }

  for (UnsignedInteger i = 0; i < otStudy.getLimitStates().getSize(); ++i)
  {
    otStudyItem->appendItem(otStudy.getLimitStates()[i]);
    otStudy.getLimitStates()[i].addObserver(otStudy.getImplementation().get());
  }

  for (UnsignedInteger i = 0; i < otStudy.getAnalyses().getSize(); ++i)
  {
    otStudyItem->appendItem(otStudy.getAnalyses()[i]);
    otStudy.getAnalyses()[i].addObserver(otStudy.getImplementation().get());
  }
  // signal for StudyTreeView to collapse models items
  emit studySubItemsAdded(otStudyItem);
}
}
