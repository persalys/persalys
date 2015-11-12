// Item.hxx

#ifndef ITEM_H
#define ITEM_H

#include <QStandardItem>

namespace OTGUI {
class Item : public QStandardItem
{
public:
  Item(const QString & name, const QString & typeName);

  virtual ~Item();

  QVariant data(int role) const;

private:
  QVariant type_;
};
}
#endif