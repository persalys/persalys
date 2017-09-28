//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
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
#ifndef OTGUI_STUDYTREEVIEW_HXX
#define OTGUI_STUDYTREEVIEW_HXX

#include "otgui/StudyTreeViewModel.hxx"
#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisWizard.hxx"
#include "otgui/DataModelDefinitionItem.hxx"
#include "otgui/DesignOfExperimentDefinitionItem.hxx"

#include <QTreeView>
#include <QMouseEvent>

namespace OTGUI {
class OTGUI_API StudyTreeView : public QTreeView
{
  Q_OBJECT

public:
  StudyTreeView(QWidget * parent = 0);

  virtual void mousePressEvent(QMouseEvent*);

public slots:
  void setAnalysisInProgress(bool);
  void showErrorMessage(QString);
  void onCustomContextMenu(const QPoint& point);
  void selectedItemChanged(const QModelIndex& currentIndex, const QModelIndex& previousIndex);

  // create objects
  void createNewOTStudy();
  void createNewAnalysis(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard=false);
  void createNewDesignOfExperimentEvaluation(const Analysis& analysis, const bool isGeneralWizard=false);

  // create windows
  void createNewOTStudyWindow(OTStudyItem* item);
  void createNewDataModelDiagramWindow(DataModelDiagramItem*);
  void createNewDataModelWindow(DataModelDefinitionItem* item, const bool createConnections=true);
  void createNewPhysicalModelDiagramWindow(PhysicalModelDiagramItem* item);
  void createNewPhysicalModelWindow(PhysicalModelDefinitionItem* item);
  void createNewProbabilisticModelWindow(ProbabilisticModelItem* item);
  void createNewDesignOfExperimentWindow(DesignOfExperimentDefinitionItem* item, const bool createConnections=true);
  void createNewLimitStateWindow(LimitStateItem* item);
  void createNewAnalysisWindow(AnalysisItem* item);
  void createAnalysisResultWindow(AnalysisItem* item);
  void createAnalysisWindow(AnalysisItem* item);

  // modify objects
  void modifyDesignOfExperiment(DesignOfExperimentDefinitionItem* item);
  void modifyDesignOfExperimentEvaluationWindow(AnalysisItem* item);
  void modifyAnalysis(AnalysisItem* item);
  void modifyStudySubItemsExpansion(OTStudyItem* item);

  // export/save/close otstudy
  void exportOTStudy();
  void saveCurrentOTStudy();
  void saveAsCurrentOTStudy();
  void saveAsOTStudy();
  void saveAsOTStudy(OTStudyItem* item, bool* notcancel=0);
  void openOTStudy(const QString& fileName="");
  void closeNotSavedOTStudyRequest(OTStudyItem* item, bool* canClose);
  bool closeOTStudy();
  bool closeAllOTStudies();
signals:
  void showWindow(OTguiSubWindow*);
  void itemSelected(QStandardItem*);
  void removeSubWindow(QStandardItem*);
  void recentFilesListChanged(const QString& recentFileName);
  void analysisInProgressStatusChanged(bool analysisInProgress);

protected:
  AnalysisWizard * getWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard=false);

private:
  StudyTreeViewModel * treeViewModel_;
  bool analysisInProgress_;
};
}
#endif
