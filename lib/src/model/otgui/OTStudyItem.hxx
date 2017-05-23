//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of an otStudy
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
#ifndef OTGUI_OTSTUDYITEM_HXX
#define OTGUI_OTSTUDYITEM_HXX

#include "otgui/PhysicalModelDiagramItem.hxx"
#include "otgui/DataModelDiagramItem.hxx"
#include "otgui/OTStudy.hxx"

namespace OTGUI {
class OTGUI_API OTStudyItem : public OTguiItem, public Observer
{
  Q_OBJECT

public:
  OTStudyItem(const OTStudy & otStudy);

  void update(Observable * source, const OT::String & message);

  void addDataModelItem(DesignOfExperiment & dataModel);
  void addPhysicalModelItem(PhysicalModel & physicalModel);
  void addDesignOfExperimentItem(DesignOfExperiment & design);
  void addLimitStateItem(LimitState & limitState);
  void addAnalysisItem(Analysis & analysis);

  void setData(const QVariant & value, int role);
  OTStudy getOTStudy() const;

  void exportOTStudy(QString fileName);

protected slots:
  virtual void requestRemove();

public slots:
  void createNewSymbolicPhysicalModel();
  void createNewPythonPhysicalModel();
#ifdef OTGUI_HAVE_YACS
  void createNewYACSPhysicalModel();
#endif
  void createNewDataModel();
  bool saveOTStudy();
  bool saveOTStudy(QString);
  bool closeOTStudy();
  void addMetaModelItem(PhysicalModel metaModel);
signals:
  void otStudyExportRequested();
  void otStudySaveAsRequested();
  void otStudySaveAsRequested(OTStudyItem* item, bool* notcancel);
  void otStudyCloseRequested(OTStudyItem* item, bool* canClose);
  void newDataModelItemCreated(DataModelDiagramItem*);
  void newPhysicalModelItemCreated(PhysicalModelDiagramItem*);
  void recentFilesListChanged(const QString & recentFileName);

protected:
  void buildActions();

private:
  OTStudy otStudy_;
  QAction * newSymbolicPhysicalModel_;
  QAction * newPythonPhysicalModel_;
#ifdef OTGUI_HAVE_YACS
  QAction * newYACSPhysicalModel_;
#endif
  QAction * newDataModel_;
  QAction * exportOTStudy_;
  QAction * saveOTStudy_;
  QAction * saveAsOTStudy_;
  QAction * closeOTStudy_;
};
}
#endif