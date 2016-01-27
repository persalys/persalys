// LimitStateItem.hxx

#ifndef LIMITSTATEITEM_H
#define LIMITSTATEITEM_H

#include "otgui/ObserverItem.hxx"
#include "otgui/LimitState.hxx"

namespace OTGUI {
class LimitStateItem : public ObserverItem
{
  Q_OBJECT

public:
  LimitStateItem(const LimitState & limitState);

  virtual ~LimitStateItem();

  LimitState getLimitState() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

public slots:
signals:
  void outputChanged();
  void outputNameChanged();
  void operatorChanged();
  void thresholdChanged();

private:
  LimitState limitState_;
};
}
#endif