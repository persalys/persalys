//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define limit states
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_LIMITSTATEWINDOW_HXX
#define PERSALYS_LIMITSTATEWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/LimitStateItem.hxx"
#include "persalys/ValueLineEdit.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QComboBox>

namespace PERSALYS
{
class PERSALYS_VIEW_API LimitStateWindow : public SubWindow
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
