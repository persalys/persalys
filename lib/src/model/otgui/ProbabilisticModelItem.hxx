// ProbabilisticModelItem.hxx

#ifndef PROBABILISTICMODELITEM_H
#define PROBABILISTICMODELITEM_H

#include "otgui/ObserverItem.hxx"
#include "otgui/InputTableModel.hxx"
#include "otgui/OutputTableModel.hxx"
#include "otgui/PhysicalModel.hxx"

namespace OTGUI {
class ProbabilisticModelItem : public ObserverItem
{
  Q_OBJECT

public:
  ProbabilisticModelItem(const PhysicalModel & physicalModel);

  virtual ~ProbabilisticModelItem();

  PhysicalModel getPhysicalModel() const;

  virtual void update(Observable * source, const std::string & message);

public slots:
  void updatePhysicalModel(const PhysicalModel & physicalModel);
signals:
  void inputChanged();
  void physicalModelChanged(const PhysicalModel&);

private:
  PhysicalModel physicalModel_;
};
}
#endif