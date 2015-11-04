// Item.hxx

#ifndef ITEM_H
#define ITEM_H

#include "otgui/Observer.hxx"

#include <QStandardItem>

namespace OTGUI {
class Item : public QObject, public QStandardItem, public Observer
{
  Q_OBJECT

public:
  Item(const std::string & name, const QString & typeName);

  virtual ~Item();

  QVariant data(int role = Qt::EditRole) const;
  void setData(const QVariant & value, int role);

private:
  QVariant type_;
  std::string name_;
};
}
#endif