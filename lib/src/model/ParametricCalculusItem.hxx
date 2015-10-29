// ParametricCalculusItem.hxx

#ifndef PARAMETRICCALCULUSITEM_H
#define PARAMETRICCALCULUSITEM_H

#include "CalculusItem.hxx"
#include "ParametricCalculus.hxx"

namespace OTGUI {
class ParametricCalculusItem : public CalculusItem
{
  Q_OBJECT

public:
  ParametricCalculusItem(const Calculus & calculus);

  virtual void update(Observable * source, const std::string & message);

signals:
  void inputSampleChanged(OT::NumericalSample);
};
}
#endif