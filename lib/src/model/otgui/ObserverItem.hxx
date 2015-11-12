// ObserverItem.hxx

#ifndef OBSERVERITEM_H
#define OBSERVERITEM_H

#include "otgui/Observer.hxx"
#include "otgui/Item.hxx"

#include <QStandardItem>

namespace OTGUI {
class ObserverItem : public QObject, public Item, public Observer
{
  Q_OBJECT

public:
  ObserverItem(const QString & name, const QString & typeName);

  virtual ~ObserverItem();

  QVariant data(int role = Qt::EditRole) const;
  void setData(const QVariant & value, int role);
};
}
#endif