#include "otgui/Item.hxx"

namespace OTGUI {

Item::Item(const QString & name, const QString & typeName)
  : QStandardItem(name)
  , type_(typeName)
{
}


Item::~Item()
{

}


QVariant Item::data(int role) const
{
  switch (role)
  {
    case Qt::UserRole:
      return type_;

    default:
      return QStandardItem::data(role);
  }
}

}