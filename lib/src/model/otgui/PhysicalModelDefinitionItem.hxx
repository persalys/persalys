//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_PHYSICALMODELDEFINITIONITEM_HXX
#define OTGUI_PHYSICALMODELDEFINITIONITEM_HXX

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/DesignOfExperiment.hxx"

namespace OTGUI {
class OTGUI_API PhysicalModelDefinitionItem : public PhysicalModelItem
{
  Q_OBJECT

public:
  PhysicalModelDefinitionItem(const PhysicalModel & physicalModel);

  virtual void update(Observable * source, const OT::String & message);

public slots:
  void createProbabilisticModel();
  void createNewDesignOfExperiment();
  void createNewModelEvaluation();
  void updateProbaActionAvailability();
signals:
  void probabilisticModelRequested(PhysicalModelItem*);
  void designOfExperimentRequested(OTguiItem*, const DesignOfExperiment&);
  void outputChanged();
  void codeChanged();
  void physicalModelRemoved(QStandardItem*);
  void parallelizeStatusChanged();
  void wantedMachineChanged();

protected:
  void buildActions();

private:
  QAction * newProbabilisticModel_;
  QAction * newModelEvaluation_;
  QAction * newDesignOfExperiment_;
};
}
#endif