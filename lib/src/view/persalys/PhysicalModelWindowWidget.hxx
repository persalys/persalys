//                                               -*- C++ -*-
/**
 *  @brief QWidget to visualize the variables of the physical model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

#include <QPushButton>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_VIEW_API PhysicalModelWindowWidget : public QTabWidget
{
  Q_OBJECT

public :
  explicit PhysicalModelWindowWidget(PhysicalModelItem * item);

protected:
  void buildInterface();
  void resizeEvent(QResizeEvent * event) override;
  void paintEvent(QPaintEvent * event) override;

public slots:
  void removeInputLine();
  void removeOutputLine();
  void evaluateOutputs();
  void resizeInputTable();
  void resizeOutputTable();
  void updateIndexParamLabel();

signals:
  void updateInputTableData();
  void updateOutputTableData();
  void updateDifferentiationTableData();
  void removeInputLine(QModelIndex);
  void removeOutputLine(QModelIndex);
  void resetMessageLabel();
  void evaluateOutputsRequested();
  void evaluateGradientRequested();
  void errorMessageChanged(QString);

private:
  PhysicalModel physicalModel_;
  bool isFirstPaint_ = true;
  CopyableTableView * inputTableView_ = nullptr;
  CopyableTableView * outputTableView_ = nullptr;
  QLabel * indexParamLabel_ = nullptr;
  OT::Scalar evalTime_ = 0.0;
};
}
#endif
