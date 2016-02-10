//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_STUDYTREEVIEW_HXX
#define OTGUI_STUDYTREEVIEW_HXX

#include "otgui/StudyTreeViewModel.hxx"

#include <QTreeView>
#include <QMdiSubWindow>
#include <QAction>

namespace OTGUI {
class StudyTreeView : public QTreeView
{
  Q_OBJECT

public:
  StudyTreeView(QWidget * parent = 0);

  QList<QAction* > getActions(const QString & dataType);

protected:
  void buildActions();

public slots:
  void createNewOTStudy();
  void createNewPhysicalModel();
  void createNewProbabilisticModel();
  void createNewDesignOfExperiment();
  void createNewLimitState();
  void createNewModelEvaluation();
  void createNewCentralTendency();
  void createNewSensitivityAnalysis();
  void createNewThresholdExceedance();
  void onCustomContextMenu(const QPoint & point);
  void selectedItemChanged(const QModelIndex & index);
  void runDesignOfExperiment();
  void runModelEvaluation();
  void runCentralTendency();
  void runSensitivityAnalysis();
  void runReliabilityAnalysis();
  void createModelEvaluationResult(AnalysisItem *);
  void createCentralTendencyResult(AnalysisItem *);
  void createSensitivityAnalysisResult(AnalysisItem * item);
  void createReliabilityAnalysisResult(AnalysisItem*);
  void createAnalysisConnection(AnalysisItem*);
  void createNewPhysicalModelWindow(PhysicalModelItem * item);
  void createNewProbabilisticModelWindow(ProbabilisticModelItem * item);
  void createNewDesignOfExperimentWindow(DesignOfExperimentItem* item);
  void createNewLimitStateWindow(LimitStateItem * item);
  void dumpOTStudy();
  void loadOTStudy();
signals:
  void showWindow(QMdiSubWindow*);
  void errorMessageEmitted(QString);
  void itemSelected(QStandardItem *);
  void checkIfWindowResultExists(ObserverItem *);
  void loadPythonScript(const QString & fileName);
  void graphWindowActivated(QWidget*);
  void graphWindowDeactivated(QWidget*);

private:
  StudyTreeViewModel * treeViewModel_;
  QAction * newOTStudyAction_;
  QAction * newPhysicalModel_;
  QAction * newProbabilisticModel_;
  QAction * newDesignOfExperiment_;
  QAction * newLimitState_;
  QAction * newModelEvaluation_;
  QAction * newCentralTendency_;
  QAction * newSensitivityAnalysis_;
  QAction * newThresholdExceedance_;
  QAction * runDesignOfExperiment_;
  QAction * runModelEvaluation_;
  QAction * runCentralTendency_;
  QAction * runSensitivityAnalysis_;
  QAction * runReliabilityAnalysis_;
  QAction * dumpOTStudy_;
};
}
#endif