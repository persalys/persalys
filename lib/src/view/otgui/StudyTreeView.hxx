// StudyTreeView.hxx

#ifndef STUDYTREEVIEW_H
#define STUDYTREEVIEW_H

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