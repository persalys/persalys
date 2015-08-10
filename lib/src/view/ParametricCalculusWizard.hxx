// ParametricCalculusWizard.hxx

#ifndef PARAMETRICCALCULUSWIZARD_H
#define PARAMETRICCALCULUSWIZARD_H

#include "ParametricCalculusTableModel.hxx"
#include "ParametricCalculusItem.hxx"

#include <QWizard>
#include <QTableView>

namespace OTGUI {
class ParametricCalculusWizard : public QWizard
{
  Q_OBJECT

public:
  ParametricCalculusWizard();
  ParametricCalculusWizard(ParametricCalculusItem * item);

  virtual ~ParametricCalculusWizard();

  ParametricCalculusItem * getItem() const;

protected:
  void buildInterface();
  void setItem(ParametricCalculusItem * item);

public slots:
  void parametrizationChanged(const QModelIndex &, const QModelIndex &);
  void completeModel(ParametricCalculusItem * item);

private:
  QTableView * tableView_;
  ParametricCalculusTableModel * model_;
  ParametricCalculusItem * item_;
};
}
#endif