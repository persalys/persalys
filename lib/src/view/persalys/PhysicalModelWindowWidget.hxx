//                                               -*- C++ -*-
/**
 *  @brief QWidget to visualize the variables of the physical model
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_PHYSICALMODELWINDOWWIDGET_HXX
#define PERSALYS_PHYSICALMODELWINDOWWIDGET_HXX

#include "persalys/PhysicalModelItem.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/TemporaryLabel.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API PhysicalModelWindowWidget : public QTabWidget
{
  Q_OBJECT

public :
  PhysicalModelWindowWidget(PhysicalModelItem * item);

protected:
  void buildInterface();
  virtual void resizeEvent(QResizeEvent * event);
  virtual void paintEvent(QPaintEvent * event);

public slots:
  void removeInputLine();
  void removeOutputLine();
  void evaluateOutputs();
  void resizeInputTable();
  void resizeOutputTable();
  void updateMultiprocessingStatus(const int nThreads);
  void updateIndexParamLabel();

signals:
  void updateInputTableData();
  void updateOutputTableData();
  void updateDifferentiationTableData();
  void removeInputLine(QModelIndex);
  void removeOutputLine(QModelIndex);
  void resetMessageLabel();

private:
  PhysicalModel physicalModel_;
  bool isFirstPaint_;
  CopyableTableView * inputTableView_;
  CopyableTableView * outputTableView_;
  TemporaryLabel * errorMessageLabel_;
  QLabel * indexParamLabel_;
  OT::Scalar evalTime_ = 0;
};
}
#endif
