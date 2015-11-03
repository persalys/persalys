// StudyTreeView.hxx


#ifndef STUDYTREEVIEW_H
#define STUDYTREEVIEW_H

#include "StudyTreeViewModel.hxx"

#include <QTreeView>
#include <QWidget>
#include <QAction>
#include <QMdiSubWindow>

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
  void createNewParametricCalculus();
  void createNewDistributionAnalysis();
  void createNewSensitivityAnalysis();
  void onCustomContextMenu(const QPoint & point);
  void selectedItemChanged(const QModelIndex & index);
  void runParametricCalculus();
  void runDistributionAnalysis();
  void runSensitivityAnalysis();
  void createParametricCalculusResult(CalculusItem *);
  void createDistributionAnalysisResult(CalculusItem *);
  void createSensitivityAnalysisResult(CalculusItem * item);
  void createCalculusConnection(CalculusItem*);
  void createNewPhysicalModelWindow(PhysicalModelItem * item);
  void dumpStudy();
  void loadStudy();
signals:
  void showWindow(QMdiSubWindow*);
  void itemSelected(QStandardItem *);
  void checkIfWindowResultExists(CalculusItem *);
  void loadPythonScript(const QString & fileName);

private:
  StudyTreeViewModel * treeViewModel_;
  QAction * newStudyAction_;
  QAction * newPhysicalModelAction_;
  QAction * newParametricCalculus_;
  QAction * newDistributionAnalysis_;
  QAction * newSensitivityAnalysis_;
  QAction * runParametricCalculus_;
  QAction * runDistributionAnalysis_;
  QAction * runSensitivityAnalysis_;
  QAction * dumpStudy_;
};
}
#endif