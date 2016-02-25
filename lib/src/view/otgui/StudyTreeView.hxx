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
  bool isPhysicalModelValid(const QModelIndex & currentIndex);
  bool hasPhysicalModelInputs(const QModelIndex & currentIndex);
  bool isProbabilisticModelValid(const QModelIndex & currentIndex);

public slots:
  void createNewOTStudy();
  void createNewAnalyticalPhysicalModel();
  void createNewPythonPhysicalModel();
#ifdef OTGUI_HAVE_YACS
  void createNewYACSPhysicalModel();
#endif
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
  void findAnalysisItemAndLaunchExecution(OTStudyItem * otStudyItem, const QString & analysisName);
  void runAnalysis();
  void removeAnalysis();
  void createNewPhysicalModelWindow(PhysicalModelItem * item);
  void createNewProbabilisticModelWindow(ProbabilisticModelItem * item);
  void createNewDesignOfExperimentWindow(DesignOfExperimentItem* item);
  void createNewLimitStateWindow(LimitStateItem * item);
  void createAnalysisConnection(AnalysisItem*);
  void createAnalysisResultWindow(AnalysisItem * item);
  void createAnalysisExecutionFailedWindow(AnalysisItem * item, const QString & errorMessage="");
  void exportPython();
  void importPython();
  void saveOTStudy();
  void saveAsOTStudy();
  void openOTStudy();
signals:
  void showWindow(QMdiSubWindow*);
  void errorMessageEmitted(QString);
  void itemSelected(QStandardItem *);
  void checkIfWindowResultExists(QStandardItem *);
  void importPythonScript(const QString & fileName);
  void graphWindowActivated(QWidget*);
  void graphWindowDeactivated(QWidget*);
  void analysisExecutionRequired(OTStudyItem * otStudyItem, const QString & analysisName);

private:
  StudyTreeViewModel * treeViewModel_;
  QAction * newOTStudyAction_;
  QAction * newAnalyticalPhysicalModel_;
  QAction * newPythonPhysicalModel_;
#ifdef OTGUI_HAVE_YACS
  QAction * newYACSPhysicalModel_;
#endif
  QAction * newProbabilisticModel_;
  QAction * newDesignOfExperiment_;
  QAction * newLimitState_;
  QAction * newModelEvaluation_;
  QAction * newCentralTendency_;
  QAction * newSensitivityAnalysis_;
  QAction * newThresholdExceedance_;
  QAction * runDesignOfExperiment_;
  QAction * runAnalysis_;
  QAction * removeAnalysis_;
  QAction * saveOTStudy_;
};
}
#endif