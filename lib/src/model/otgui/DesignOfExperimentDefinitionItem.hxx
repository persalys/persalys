//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of design of experiments
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

#include "otgui/AnalysisItem.hxx"

namespace OTGUI
{
class OTGUI_API DesignOfExperimentDefinitionItem : public OTguiItem, public Observer
{
  Q_OBJECT

public:
  DesignOfExperimentDefinitionItem(const Analysis& analysis);

  void setData(const QVariant & value, int role);

  OT::Sample getOriginalInputSample() const;
  Analysis getAnalysis() const;
  void updateAnalysis(const Analysis & analysis);

  void appendAnalysisItem(Analysis& analysis);

  virtual void update(Observable * source, const OT::String & message);
  void fill();

protected:
  void buildActions();

public slots:
  void appendEvaluationItem();
  void createNewMetaModel();
  void modifyAnalysis();
  void createNewEvaluation();
  void removeAnalysis();

signals:
  void modifyAnalysisRequested(DesignOfExperimentDefinitionItem*);
  void DOEEvaluationRequested(const Analysis&, const bool isGeneralWizard = false);
  void updateEvaluationWindowRequested(AnalysisItem*);
  void newAnalysisItemCreated(AnalysisItem*);

  void numberDesignEvaluationChanged(bool);
  void designEvaluationAppended();

private:
  Analysis analysis_;
  QAction * newMetaModel_;
  QAction * modifyAnalysis_;
  QAction * evaluateDesignOfExperiment_;
  QAction * removeAnalysis_;
};
}
#endif
