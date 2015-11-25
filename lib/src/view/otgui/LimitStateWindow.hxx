// LimitStateWindow.hxx

#ifndef LIMITSTATEWINDOW_H
#define LIMITSTATEWINDOW_H

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/LimitStateItem.hxx"

#include <QComboBox>
#include <QDoubleSpinBox>

namespace OTGUI {
class LimitStateWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  enum Operator {LessOperator, GreaterOperator};

  LimitStateWindow(LimitStateItem * item);

protected:
  void buildInterface();

public slots:
  void updateOutputsList();
  void updateOutputWidget();
  void updateOperatorWidget();
  void updateThresholdWidget();
  void updateOutput(int);
  void updateOperator(int);
  void updateThreshold(double);
signals:
  void limitStateChanged(const LimitState & limitState);

private:
  LimitState limitState_;

  QComboBox * outputsComboBox_;
  QComboBox * failureComboBox_;
  QDoubleSpinBox * thresholdSpinBox_;
};
}
#endif