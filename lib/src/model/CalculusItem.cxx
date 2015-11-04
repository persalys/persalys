// CalculusItem.cxx

#include "otgui/CalculusItem.hxx"

namespace OTGUI {

CalculusItem::CalculusItem(const Calculus & calculus, const QString & typeCalculus)
  : Item(calculus.getName(), typeCalculus)
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
      calculus_.setName(value.toString().toStdString());
      Item::setData(value, role);
  }
}


Calculus CalculusItem::getCalculus() const
{
  return calculus_;
}


void CalculusItem::updateCalculus(const Calculus & calculus)
{
  calculus_ = calculus;
  calculus_.addObserver(this);
}


}