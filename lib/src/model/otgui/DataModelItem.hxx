//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
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
#ifndef OTGUI_DATAMODELITEM_HXX
#define OTGUI_DATAMODELITEM_HXX

#include "otgui/DataModel.hxx"

#include <QStandardItem>

namespace OTGUI {
class OTGUI_API DataModelItem : public QObject, public QStandardItem, public Observer
{
  Q_OBJECT

public:
  DataModelItem(const DataModel & dataModel);

  DataModel getDataModel() const;

  void setData(const QVariant & value, int role);
  void updateDataModel(const DataModel & dataModel);
  virtual void update(Observable * source, const OT::String & message);

public slots:
signals:
  void dataModelChanged(const DataModel&);
  void dataModelRemoved(QStandardItem*);

private:
  DataModel dataModel_;
};
}
#endif