//                                               -*- C++ -*-
/**
 *  @brief QObject managing study objects
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_STUDYMANAGER_HXX
#define PERSALYS_STUDYMANAGER_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/AnalysisWizard.hxx"
#include "persalys/DesignOfExperimentItem.hxx"
#include "persalys/DesignOfExperimentDefinitionItem.hxx"
#include "persalys/MainWidget.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API StudyManager : public QObject
{
  Q_OBJECT

public:
  StudyManager(MainWidget * mainWidget, QObject * parent = 0);

  bool analysisInProgress() const;

public slots:
  void showErrorMessage(const QString&);

  void createStudy();

  // open wizards
  void openAnalysisWizard(StudyItem* item, const Analysis& analysis, const bool isGeneralWizard = false);
  void openDesignOfExperimentEvaluationWizard(const Analysis& analysis, const bool isGeneralWizard = false);
  void openObservationsWizard(StudyItem*, const DesignOfExperiment& designOfExp);
  void openExtractDataFieldWizard(StudyItem *item, const Analysis& analysis);
  void modifyAnalysis(AnalysisItem* item);

  // create windows
  void createWindow(Item *item);
  void createAnalysisWindow(AnalysisItem* item, const bool createConnections = true);

  // import/export/save/open/close study
  void importPythonScript(const QString& fileName="");
  void exportPythonScript();
  void saveCurrent();
  void saveAsCurrent();
  bool save(StudyItem* item);
  bool saveAs(StudyItem* item);
  void open(const QString& fileName = "");
  bool close(StudyItem* item);
  bool closeCurrent();
  bool closeAll();
signals:
  void recentFilesListChanged(const QString& recentFileName);
  void commandExecutionRequested(const QString& command);

protected:
  void updateView(SubWindow * window);

private:
  MainWidget * mainWidget_;
};
}
#endif
