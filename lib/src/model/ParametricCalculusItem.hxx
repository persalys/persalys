// ParametricCalculusItem.hxx

#ifndef PARAMETRICCALCULUSITEM_H
#define PARAMETRICCALCULUSITEM_H

#include "CalculusItem.hxx"
#include "InputTableModel.hxx"
#include "ParametricCalculus.hxx"
#include "ParametricCalculusTableModel.hxx"

namespace OTGUI {
class ParametricCalculusItem : public CalculusItem
{
  Q_OBJECT

public:
  ParametricCalculusItem(ParametricCalculus * calculus);
  virtual ~ParametricCalculusItem();

  virtual void update(Observable * source, const std::string & message);

public slots:
signals:
  void inputSampleChanged(OT::NumericalSample);
};
}
#endif