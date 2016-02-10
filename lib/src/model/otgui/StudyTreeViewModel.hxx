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
#ifndef OTGUI_STUDYTREEVIEWMODEL_HXX
#define OTGUI_STUDYTREEVIEWMODEL_HXX

#include "otgui/OTStudyItem.hxx"

#include <QStandardItemModel>

namespace OTGUI {
class StudyTreeViewModel : public QStandardItemModel, public Observer
{
  Q_OBJECT

public:
  StudyTreeViewModel();

  virtual ~StudyTreeViewModel();

  void createNewOTStudy();
  void addOTStudyItem(OTStudy * otStudy);
  void addPhysicalModelItem(const QModelIndex & parentIndex);
  void addProbabilisticModelItem(const QModelIndex & parentIndex);
  void addLimitStateItem(const QModelIndex & parentIndex);

  OTStudyItem * getOTStudyItem(const QModelIndex & childIndex);

  virtual void update(Observable * source, const OT::String & message);

public slots:
signals:
  void newPhysicalModelCreated(PhysicalModelItem*);
  void newProbabilisticModelCreated(ProbabilisticModelItem*);
  void newDesignOfExperimentCreated(DesignOfExperimentItem*);
  void newLimitStateCreated(LimitStateItem*);
  void newAnalysisCreated(AnalysisItem*);
};
}
#endif