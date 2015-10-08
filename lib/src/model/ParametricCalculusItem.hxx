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
  virtual ~ParametricCalculusItem();

  virtual void update(Observable * source, const std::string & message);

public slots:
  void updateCalculus(InputCollection);
signals:
  void inputSampleChanged(OT::NumericalSample);

private:
  Calculus calculus_;
};
}
#endif