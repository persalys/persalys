//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a study
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_STUDYITEM_HXX
#define OTGUI_STUDYITEM_HXX

#include "otgui/PhysicalModelDiagramItem.hxx"
#include "otgui/DataModelDiagramItem.hxx"
#include "otgui/Study.hxx"

namespace OTGUI
{
class OTGUI_API StudyItem : public Item, public Observer
{
  Q_OBJECT

public:
  StudyItem(const Study & study);

  void update(Observable * source, const OT::String & message);

  void appendItem(DesignOfExperiment & dataModel);
  void appendItem(PhysicalModel & physicalModel);
  void appendItem(LimitState & limitState);
  void appendItem(Analysis & analysis);

  virtual QVariant data(int role) const;
  void setData(const QVariant & value, int role);
  Study getStudy() const;

  void exportPythonScript(const QString& fileName);

protected slots:
  virtual void requestRemove();

public slots:
  void updateIcon();
  void createSymbolicModel();
  void createSymbolicFieldModel();
  void createPythonModel();
  void createPythonFieldModel();
#ifdef OTGUI_HAVE_YACS
  void createYACSModel();
#endif
#ifdef OTGUI_HAVE_OTFMI
  void createFMIModel();
#endif
  void createDataModel();
  void emitSave();
  void emitSaveAs();
  bool save(const QString&);
  void emitClose();
  void appendMetaModelItem(PhysicalModel metaModel);
signals:
  void statusChanged();
  void exportRequested();
  void saveRequested();
  void saveAsRequested();
  void saveRequested(StudyItem* item);
  void saveAsRequested(StudyItem* item);
  void closeRequested(StudyItem* item);
  void dataModelItemCreated(DataModelDiagramItem*);
  void physicalModelItemCreated(PhysicalModelDiagramItem*);
  void fieldModelItemCreated(PhysicalModelDiagramItem*);

protected:
  void buildActions();

private:
  Study study_;
  QAction * newSymbolicModel_;
  QAction * newPythonModel_;
#ifdef OTGUI_HAVE_YACS
  QAction * newYACSModel_;
#endif
#ifdef OTGUI_HAVE_OTFMI
  QAction * newFMIModel_;
#endif
  QAction * newSymbolicFieldModel_;
  QAction * newPythonFieldModel_;
  QAction * newDataModel_;
  QAction * exportAction_;
  QAction * saveAction_;
  QAction * saveAsAction_;
  QAction * closeAction_;
};
}
#endif
