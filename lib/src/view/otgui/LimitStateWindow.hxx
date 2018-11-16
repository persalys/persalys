//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define limit states
 *
 *  Copyright 2015-2018 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_LIMITSTATEWINDOW_HXX
#define OTGUI_LIMITSTATEWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/LimitStateItem.hxx"
#include "otgui/ValueLineEdit.hxx"
#include "otgui/TemporaryLabel.hxx"

#include <QComboBox>

namespace OTGUI
{
class OTGUI_API LimitStateWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  enum Operator {LessOperator, LessOrEqualOperator, GreaterOperator, GreaterOrEqualOperator};

  LimitStateWindow(LimitStateItem * item, QWidget * parent = 0);

protected:
  void buildInterface();

public slots:
  void updateOutputsList();
  void updateOutputWidget();
  void updateOperatorWidget();
  void updateThresholdWidget();
  void updateOutput(int);
  void updateOperator(int);
  void updateThreshold();
signals:
  void limitStateChanged(const LimitState & limitState);

private:
  LimitState limitState_;

  QComboBox * outputsComboBox_;
  QComboBox * failureComboBox_;
  ValueLineEdit * thresholdLineEdit_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
