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
  virtual ~StudyTreeView();

protected:
  void buildActions();
  void buildContextMenus();

public slots:
  void createNewStudy();
  void createNewPhysicalModel();
  void createNewParametricCalculus();
  void onCustomContextMenu(const QPoint & point);
  void selectedItemChanged(const QModelIndex & index);
  void runParametricCalculus();
  void createParametricCalculusResult(CalculusItem *);
  void createParametricCalculusConnection(ParametricCalculusItem*);
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
  QAction * runParametricCalculus_;
  QAction * dumpStudy_;
};
}
#endif