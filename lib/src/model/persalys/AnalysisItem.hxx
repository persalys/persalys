//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_ANALYSISITEM_HXX
#define PERSALYS_ANALYSISITEM_HXX

#include "persalys/Item.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/MetaModel.hxx"
#include "persalys/DesignOfExperiment.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API AnalysisItem : public Item, public Observer
{
  Q_OBJECT

public:
  AnalysisItem(const Analysis & analysis);
  AnalysisItem(const Analysis & analysis, const OT::String &typeName);

  QVariant data(int role) const override;
  void setData(const QVariant & value, int role) override;

  Analysis getAnalysis() const;

  void update(Observable * source, const OT::String & message) override;

protected:
  virtual void buildActions();
  void addDoEToStudy(const StudyItem * studyItem, const OT::String &name, const PhysicalModel &model, const DesignOfExperiment &doe) const;

public slots:
  void processStatusChanged();
  virtual void updateAnalysis(const Analysis & analysis);
  void stopAnalysis();
  void interruptAnalysis();
  void detachAnalysis();
  void modifyAnalysis();
  void addPythonMetaModel();
  void appendDataModelItem();
  virtual void removeAnalysis();
  void extractData();

signals:
  void analysisRemoved(QStandardItem*);
  void messageChanged(QString);
  void progressValueChanged(int);
  void modifyAnalysisRequested(AnalysisItem*);
  void dataExtractionWizardRequested(StudyItem*, Analysis);
  void pythonMetamodelExportRequested(PhysicalModel);
  void evaluationsImportRequested(const StudyItem*, const Analysis&);

  void numberDesignEvaluationChanged(bool);
  void designEvaluationUpdated(bool);
  void numberMetamodelChanged(int);

protected:
  Analysis analysis_;
  QAction * modifyAction_ = nullptr;
  QAction * convertAction_ = nullptr;
  QAction * exportAction_ = nullptr;
  QAction * removeAction_ = nullptr;
  QAction * addEvaluationsAction_ = nullptr;

private:
  QAction * extractDataAction_ = nullptr;
  QAction * convertPythonAction_ = nullptr;
};
}
#endif
