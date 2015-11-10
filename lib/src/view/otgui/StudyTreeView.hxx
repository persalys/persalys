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

protected:
  void buildActions();

public slots:
  void createNewStudy();
  void createNewPhysicalModel();
  void createNewParametricAnalysis();
  void createNewCentralTendency();
  void createNewSensitivityAnalysis();
  void onCustomContextMenu(const QPoint & point);
  void selectedItemChanged(const QModelIndex & index);
  void runParametricAnalysis();
  void runCentralTendency();
  void runSensitivityAnalysis();
  void createParametricAnalysisResult(AnalysisItem *);
  void createCentralTendencyResult(AnalysisItem *);
  void createSensitivityAnalysisResult(AnalysisItem * item);
  void createAnalysisConnection(AnalysisItem*);
  void createNewPhysicalModelWindow(PhysicalModelItem * item);
  void dumpStudy();
  void loadStudy();
signals:
  void showWindow(QMdiSubWindow*);
  void itemSelected(QStandardItem *);
  void checkIfWindowResultExists(AnalysisItem *);
  void loadPythonScript(const QString & fileName);

private:
  StudyTreeViewModel * treeViewModel_;
  QAction * newStudyAction_;
  QAction * newPhysicalModelAction_;
  QAction * newParametricAnalysis_;
  QAction * newCentralTendency_;
  QAction * newSensitivityAnalysis_;
  QAction * runParametricAnalysis_;
  QAction * runCentralTendency_;
  QAction * runSensitivityAnalysis_;
  QAction * dumpStudy_;
};
}
#endif