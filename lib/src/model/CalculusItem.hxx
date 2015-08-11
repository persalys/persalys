// CalculusItem.hxx

#ifndef CALCULUSITEM_H
#define CALCULUSITEM_H

#include "Item.hxx"
#include "Calculus.hxx"

namespace OTGUI {
class CalculusItem : public Item
{
  Q_OBJECT

public:
  CalculusItem(const Calculus & calculus, const QString & typeCalculus=QString("Calculus"));

  virtual ~CalculusItem();

  void setData(const QVariant & value, int role);

  Calculus getCalculus() const;
  void setCalculus(const Calculus & calculus);

public slots:
signals:
  void calculusFinished(CalculusItem*);

private:
  Calculus calculus_;
};
}
#endif