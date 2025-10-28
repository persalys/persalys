//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
class PERSALYS_MODEL_API DataModelDiagramItem : public DesignOfExperimentItem
{
  Q_OBJECT

  friend class DataModelDefinitionItem;
  friend class DataModelDiagramWindow;

public:
  explicit DataModelDiagramItem(const DesignOfExperiment& designOfExperiment);

  void update(Observable* source, const OT::String& message) override;

  void fill();
  void appendItem(const Analysis& analysis) override;

protected:
  void buildActions();

public slots:
  void appendDataModelItem();
  void removeDesignOfExperiment();
  void updateMetamodelCounter(int);
  void requestMetaModelExport();

signals:
  // signal for diagram
  void dataSizeValidityChanged(bool, QString);
  void dataModelValidityChanged(bool, QString);
  void dependenciesValidityChanged(bool, QString);
  void metaModelValidityChanged(bool, QString);
  void dataSensitivityValidityChanged(bool, QString);
  void metamodelNumberValidityChanged(bool, QString);

private:
  QAction * defineAction_               = nullptr;
  QAction * newDataAnalysis_            = nullptr;
  QAction * newDataSensitivityAnalysis_ = nullptr;
  QAction * newQuantileAnalysis_        = nullptr;
  QAction * newInferenceAnalysis_       = nullptr;
  QAction * newCopulaInferenceAnalysis_ = nullptr;
  QAction * newMetaModel_               = nullptr;
  QAction * removeAction_               = nullptr;

  int metamodelCounter_ = 0;
};
}
#endif
