//                                               -*- C++ -*-
/**
 *  @brief QWidget to visualize the variables of the physical model
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
#ifndef OTGUI_PHYSICALMODELWINDOWWIDGET_HXX
#define OTGUI_PHYSICALMODELWINDOWWIDGET_HXX

#include "otgui/PhysicalModelDefinitionItem.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/TemporaryLabel.hxx"

namespace OTGUI
{
class OTGUI_API PhysicalModelWindowWidget : public QTabWidget
{
  Q_OBJECT

public :
  PhysicalModelWindowWidget(PhysicalModelDefinitionItem * item);

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
  void updateMultiprocessingStatus(int);

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
};
}
#endif
