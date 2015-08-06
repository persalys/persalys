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

  void runCalculus();
  void setCalculus(ParametricCalculus * calculus);
  OT::NumericalSample getInputSample() const;
  OT::NumericalSample getResultSample() const;
  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const std::string & message);

public slots:
signals:
  void inputSampleChanged(OT::NumericalSample);

private:
  ParametricCalculus * calculus_;
};
}
#endif