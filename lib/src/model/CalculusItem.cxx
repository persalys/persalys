// CalculusItem.cxx

#include "CalculusItem.hxx"

namespace OTGUI {

CalculusItem::CalculusItem(Calculus * calculus, const QString & typeCalculus)
  : Item(calculus->getName(), typeCalculus)
  , calculus_(calculus)
{
}


CalculusItem::~CalculusItem()
{
}



void CalculusItem::setData(const QVariant& value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      calculus_->setName(value.toString().toStdString());
      Item::setData(value, role);
  }
}


Calculus* CalculusItem::getCalculus() const
{
  return calculus_;
}


void CalculusItem::setCalculus(Calculus* calculus)
{
  calculus_ = calculus;
}


}