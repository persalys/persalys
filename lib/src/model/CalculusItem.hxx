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
  CalculusItem(Calculus * calculus, const QString & typeCalculus=QString("Calculus"));

  virtual ~CalculusItem();

  void setData(const QVariant & value, int role);

  Calculus * getCalculus() const;
  template <class CalculusT> CalculusT* getCalculus() const
  {
    Q_ASSERT(calculus_);
    CalculusT* c = dynamic_cast<CalculusT*>(calculus_);
    Q_ASSERT(c);
    return c;
  }
  void setCalculus(Calculus * calculus);

public slots:
signals:
  void calculusFinished(CalculusItem*);

private:
  Calculus * calculus_;
};
}
#endif