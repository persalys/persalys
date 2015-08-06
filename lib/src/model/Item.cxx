#include "Item.hxx"

#include <iostream>

namespace OTGUI {
  

Item::Item(const std::string & name, const QString & typeName)
  : QStandardItem(QString::fromStdString(name))
  , Observer()
  , type_(QVariant(typeName))
  , name_(name)
{

}


Item::~Item()
{

}

//method to search good name for the item in accordance with its type


QVariant Item::data(int role) const
{
  switch (role)
  {
    case Qt::UserRole:
      return type_;
      break;

    case Qt::DisplayRole:
    case Qt::EditRole:
      return name_.c_str();
      break;

    default:
      return QVariant();
  }
  return QVariant();
}


void Item::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      name_ = value.toString().toStdString();
      emitDataChanged();
  }
}

}