#include "otgui/LimitStateItem.hxx"

namespace OTGUI {

LimitStateItem::LimitStateItem(const LimitState & limitState)
  : ObserverItem(limitState.getName().c_str(), QString("LimitState"))
  , limitState_(limitState)
{
}


LimitStateItem::~LimitStateItem()
{
}


void LimitStateItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      limitState_.setName(value.toString().toStdString());
      ObserverItem::setData(value, role);
      break;
  }
}


LimitState LimitStateItem::getLimitState() const
{
  return limitState_;
}


void LimitStateItem::update(Observable* source, const std::string & message)
{
  if (message=="updateLimitStateWindow")
  {
    emit outputChanged();
  }
  else if (message=="outputNameChanged")
  {
    emit outputNameChanged();
  }
  else if (message=="operatorChanged")
  {
    emit operatorChanged();
  }
  else if (message=="thresholdChanged")
  {
    emit thresholdChanged();
  }
}
}