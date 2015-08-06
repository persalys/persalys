// ParametricCalculusWizard.hxx

#ifndef PARAMETRICCALCULUSWIZARD_H
#define PARAMETRICCALCULUSWIZARD_H

#include "ParametricCalculusTableModel.hxx"
#include "ParametricCalculusItem.hxx"

#include <QWizard>

namespace OTGUI {
class ParametricCalculusWizard : public QWizard
{
  Q_OBJECT

public:
  ParametricCalculusWizard(ParametricCalculusItem * item);

  virtual ~ParametricCalculusWizard();

protected:
  void buildInterface();

public slots:
  void parametrizationChanged(const QModelIndex &, const QModelIndex &);

private:
  ParametricCalculusTableModel * model_;
  ParametricCalculusItem * item_;
};
}
#endif