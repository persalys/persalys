//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_DATAMODELDEFINITIONITEM_HXX
#define PERSALYS_DATAMODELDEFINITIONITEM_HXX

#include "persalys/DesignOfExperimentItem.hxx"

namespace PERSALYS
{
class PERSALYS_API DataModelDefinitionItem : public DesignOfExperimentItem
{
  Q_OBJECT

public:
  DataModelDefinitionItem(const DesignOfExperiment & designOfExperiment);

  virtual void update(Observable * source, const OT::String & message);

protected:
  void buildActions();
  bool designOfExperimentValid();

public slots:
  void createDataAnalysis();
  void createInferenceAnalysis();
  void createCopulaInferenceAnalysis();
  void createMetaModel();
signals:
  void variablesChanged();

private:
  QAction * newDataAnalysis_;
  QAction * newInferenceAnalysis_;
  QAction * newCopulaInferenceAnalysis_;
  QAction * newMetaModel_;
};
}
#endif