//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTITEM_HXX
#define OTGUI_DESIGNOFEXPERIMENTITEM_HXX

#include "otgui/ObserverItem.hxx"
#include "otgui/DesignOfExperiment.hxx"

namespace OTGUI {
class DesignOfExperimentItem : public ObserverItem
{
  Q_OBJECT

public:
  DesignOfExperimentItem(const DesignOfExperiment & designOfExperiment);

  virtual ~DesignOfExperimentItem();

  DesignOfExperiment getDesignOfExperiment() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

public slots:
signals:
  void analysisFinished();

private:
  DesignOfExperiment designOfExperiment_;
};
}
#endif