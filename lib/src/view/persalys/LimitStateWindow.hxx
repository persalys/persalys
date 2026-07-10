//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define limit states
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/ErrorWidget.hxx"

#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>

namespace PERSALYS
{
class PERSALYS_VIEW_API LimitStateWindow : public SubWindow
{
  Q_OBJECT

public:
  enum Operator {LessOperator, LessOrEqualOperator, GreaterOperator, GreaterOrEqualOperator};

  explicit LimitStateWindow(LimitStateItem * item, QWidget *parent = nullptr);

protected:
  void buildInterface();

public slots:
  void rebuildEventsTable();
  void updateTypeWidget();
  void addFailureEvent();
  void removeFailureEvent(int row);
  void updateType(int index);

signals:
  void limitStateChanged(const LimitState & limitState);

private:
  void updateOutput(int row, int comboIndex);
  void updateOperator(int row, int comboIndex);
  void updateThreshold(int row, ValueLineEdit * edit);

  LimitState limitState_;

  QWidget       * typeWidget_      = nullptr;
  QComboBox     * typeComboBox_    = nullptr;
  QTableWidget  * eventsTable_     = nullptr;
  QPushButton   * addEventButton_  = nullptr;
  ErrorWidget   * errorWidget_     = nullptr;
};
}
#endif
