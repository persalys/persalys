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
#ifndef PERSALYS_STUDYTREEVIEWMODEL_HXX
#define PERSALYS_STUDYTREEVIEWMODEL_HXX

#include "persalys/StudyItem.hxx"

#include <QStandardItemModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API StudyTreeViewModel : public QStandardItemModel, public Observer
{
  Q_OBJECT

public:
  StudyTreeViewModel(QObject *parent = 0);

  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  using Observer::appendItem;
  void appendItem(const Study & study);

  virtual void update(Observable * source, const OT::String & message);

signals:
  void windowRequested(Item*);
  void studySubItemsAdded(StudyItem*);

public:
  static const int DefaultSignificantDigits;
};
}
#endif
