//                                               -*- C++ -*-
/**
 *  @brief QObject managing otStudy objects
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
#ifndef OTGUI_STUDYMANAGER_HXX
#define OTGUI_STUDYMANAGER_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisWizard.hxx"
#include "otgui/OTStudyItem.hxx"
#include "otgui/DataModelDefinitionItem.hxx"
#include "otgui/DesignOfExperimentDefinitionItem.hxx"
#include "otgui/MainWidget.hxx"

namespace OTGUI
{
class OTGUI_API StudyManager : public QObject
{
  Q_OBJECT

public:
  StudyManager(MainWidget * mainWidget, QObject * parent = 0);

public slots:
  void setAnalysisInProgress(bool);
  void showErrorMessage(QString);

  // create objects
  void createNewOTStudy();
  void openAnalysisWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard = false);
  void openDesignOfExperimentEvaluationWizard(const Analysis& analysis, const bool isGeneralWizard = false);

  // create windows
  void createNewOTStudyWindow(OTStudyItem* item);
  void createNewDataModelDiagramWindow(DataModelDiagramItem*);
  void createNewDataModelWindow(DataModelDefinitionItem* item);
  void createNewPhysicalModelDiagramWindow(PhysicalModelDiagramItem* item);
  void createNewPhysicalModelWindow(PhysicalModelDefinitionItem* item);
  void createNewProbabilisticModelWindow(ProbabilisticModelItem* item);
  void createNewDesignOfExperimentWindow(DesignOfExperimentDefinitionItem* item, const bool createConnections = true);
  void createNewLimitStateWindow(LimitStateItem* item);
  void createNewAnalysisWindow(AnalysisItem* item);
  void createAnalysisWindow(AnalysisItem* item);

  // modify objects
  void modifyDesignOfExperiment(DesignOfExperimentDefinitionItem* item);
  void modifyAnalysis(AnalysisItem* item);

  // export/save/close otstudy
  void importPythonScript();
  void exportPythonScript();
  void saveCurrent();
  void saveAsCurrent();
  bool save(OTStudyItem* item);
  bool saveAs(OTStudyItem* item);
  void open(const QString& fileName = "");
  bool close(OTStudyItem* item);
  bool closeCurrent();
  bool closeAll();
signals:
  void showWindow(OTguiSubWindow*);
  void recentFilesListChanged(const QString& recentFileName);
  void analysisInProgressStatusChanged(bool analysisInProgress);
  void commandExecutionRequested(const QString& command);

protected:
  void updateView(OTguiSubWindow * window);
//   AnalysisWizard * getWizard(const Analysis& analysis, const bool isGeneralWizard = false);
//   OTguiSubWindow * getAnalysisResultWindow(AnalysisItem* item);

private:
  MainWidget * mainWidget_;
  bool analysisInProgress_;
};
}
#endif
