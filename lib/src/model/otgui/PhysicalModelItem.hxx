// PhysicalModelItem.hxx

#ifndef PHYSICALMODELITEM_H
#define PHYSICALMODELITEM_H

#include "otgui/ObserverItem.hxx"
#include "otgui/InputTableModel.hxx"
#include "otgui/OutputTableModel.hxx"
#include "otgui/PhysicalModel.hxx"

namespace OTGUI {
class PhysicalModelItem : public ObserverItem
{
  Q_OBJECT

public:
  PhysicalModelItem(const PhysicalModel & physicalModel);

  virtual ~PhysicalModelItem();

  PhysicalModel getPhysicalModel() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const std::string & message);

public slots:
  void updatePhysicalModel(const PhysicalModel & physicalModel);

public slots:
signals:
  void inputChanged();
  void outputChanged();
  void physicalModelChanged(const PhysicalModel&);

private:
  PhysicalModel physicalModel_;
};
}
#endif