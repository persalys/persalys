//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTDEFINITIONITEM_HXX
#define OTGUI_DESIGNOFEXPERIMENTDEFINITIONITEM_HXX

#include "otgui/DesignOfExperimentItem.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentDefinitionItem : public DesignOfExperimentItem
{
  Q_OBJECT

public:
  DesignOfExperimentDefinitionItem(const DesignOfExperiment & designOfExperiment);

  virtual void updateDesignOfExperiment(const DesignOfExperiment & designOfExperiment);
  virtual void update(Observable * source, const OT::String & message);

  void fill();

protected:
  void buildActions();

public slots:
  void modifyDesignOfExperiment();
  void appendEvaluationItem();
signals:
  void modifyDesignOfExperimentRequested(DesignOfExperimentDefinitionItem*);
  void numberDesignEvaluationChanged(bool);
  void designEvaluationAppended();

private:
  QAction * modifyDesignOfExperiment_;
  QAction * evaluateDesignOfExperiment_;
  QAction * newMetaModel_;
  QAction * removeDesignOfExperiment_;
};
}
#endif