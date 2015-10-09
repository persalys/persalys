// PhysicalModelItem.hxx

#ifndef PHYSICALMODELITEM_H
#define PHYSICALMODELITEM_H

#include "Item.hxx"
#include "InputTableModel.hxx"
#include "OutputTableModel.hxx"
#include "PhysicalModel.hxx"

namespace OTGUI {
class PhysicalModelItem : public Item
{
  Q_OBJECT

public:
  PhysicalModelItem(const PhysicalModel & physicalModel);

  virtual ~PhysicalModelItem();

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const std::string & message);

public slots:
signals:
  void inputChanged(InputCollection);
  void outputChanged(OutputCollection);

private:
  PhysicalModel physicalModel_;
};
}
#endif