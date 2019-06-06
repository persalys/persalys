//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
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
#ifndef PERSALYS_DATAMODELDIAGRAMITEM_HXX
#define PERSALYS_DATAMODELDIAGRAMITEM_HXX

#include "persalys/DataModelDefinitionItem.hxx"
#include "persalys/AnalysisItem.hxx"

namespace PERSALYS
{
class PERSALYS_API DataModelDiagramItem : public DesignOfExperimentItem
{
  Q_OBJECT

public:
  DataModelDiagramItem(const DesignOfExperiment& designOfExperiment);

  virtual void update(Observable* source, const OT::String& message);

  void fill();
  void appendItem(Analysis& analysis);

protected:
  void buildActions();

public slots:
  void appendDataModelItem();
  void removeDesignOfExperiment();
signals:
  // signals for StudyTreeview
  void modelDefinitionWindowRequested(DataModelDefinitionItem*);
  void analysisItemCreated(AnalysisItem*);
  void dataAnalysisRequested();
  void inferenceRequested();
  void copulaInferenceRequested();
  void metaModelRequested();

  void changeCurrentItemRequested(QModelIndex);

  // signal for diagram
  void dataModelValidityChanged(bool);
  void dependenciesValidityChanged(bool);
  void metaModelValidityChanged(bool);

private:
  QAction * defineAction_;
  QAction * removeAction_;
};
}
#endif