//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a study
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
#ifndef PERSALYS_STUDYITEM_HXX
#define PERSALYS_STUDYITEM_HXX

#include "persalys/PhysicalModelDiagramItem.hxx"
#include "persalys/DataModelDiagramItem.hxx"
#include "persalys/Study.hxx"

namespace PERSALYS
{
class PERSALYS_API StudyItem : public Item, public Observer
{
  Q_OBJECT

  friend class StudyWindow;

public:
  StudyItem(const Study & study);

  void update(Observable * source, const OT::String & message);

  virtual void appendItem(const DesignOfExperiment & dataModel);
  void appendItem(const PhysicalModel & physicalModel);
  virtual void appendItem(const LimitState & limitState);
  virtual void appendItem(const Analysis & analysis);

  virtual QVariant data(int role) const;
  void setData(const QVariant & value, int role);
  Study getStudy() const;

  void exportPythonScript(const QString& fileName);

protected slots:
  virtual void requestRemove();

public slots:
  void updateIcon();
  void emitSave();
  void emitSaveAs();
  bool save(const QString&);
  void emitClose();
  void appendMetaModelItem(PhysicalModel metaModel);
signals:
  void statusChanged();

protected:
  void buildActions();

private:
  OT::String getModelName(const QString &baseName) const;
  Study study_;
  QAction * exportAction_ = 0;
  QAction * saveAction_ = 0;
  QAction * saveAsAction_ = 0;
  QAction * closeAction_ = 0;
};
}
#endif
