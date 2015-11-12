#include "otgui/ObserverItem.hxx"

namespace OTGUI {

ObserverItem::ObserverItem(const QString & name, const QString & typeName)
  : QObject()
  , Item(name, typeName)
  , Observer()
{
}


ObserverItem::~ObserverItem()
{

}

//method to search good name for the item in accordance with its type


QVariant ObserverItem::data(int role) const
{
  switch (role)
  {
    case Qt::EditRole:
      return Item::data(Qt::DisplayRole);

    default:
      return Item::data(role);
  }
}


void ObserverItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      QStandardItem::setData(value.toString(), role);
      emitDataChanged();
      break;
  }
}
}