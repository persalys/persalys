//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
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
#ifndef PERSALYS_ANALYSISITEM_HXX
#define PERSALYS_ANALYSISITEM_HXX

#include "persalys/Item.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/MetaModel.hxx"
#include "persalys/DesignOfExperiment.hxx"

namespace PERSALYS
{
class PERSALYS_API AnalysisItem : public Item, public Observer
{
  Q_OBJECT

public:
  AnalysisItem(const Analysis & analysis);
  AnalysisItem(const Analysis & analysis, const OT::String &typeName);

  virtual QVariant data(int role) const;
  void setData(const QVariant & value, int role);

  Analysis getAnalysis() const;

  virtual void update(Observable * source, const OT::String & message);

protected:
  virtual void buildActions();

public slots:
  void processStatusChanged();
  virtual void updateAnalysis(const Analysis & analysis);
  void stopAnalysis();
  void modifyAnalysis();
  void appendMetaModelItem();
  void appendDataModelItem();
  virtual void removeAnalysis();
  void extractData();
signals:
  void analysisRemoved(QStandardItem*);
  void messageChanged(QString);
  void progressValueChanged(int);
  void modifyAnalysisRequested(AnalysisItem*);
  void dataExtractionWizardRequested(StudyItem*, Analysis);

  void numberDesignEvaluationChanged(bool);
  void designEvaluationUpdated(bool);

protected:
  Analysis analysis_;
  QAction * modifyAction_ = 0;
  QAction * convertAction_ = 0;
  QAction * removeAction_ = 0;
private:
  QAction * extractDataAction_ = 0;
};
}
#endif
