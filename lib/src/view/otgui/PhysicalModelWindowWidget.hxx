//                                               -*- C++ -*-
/**
 *  @brief QWidget to visualize the variables of the physical model
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_PHYSICALMODELWINDOWWIDGET_HXX
#define OTGUI_PHYSICALMODELWINDOWWIDGET_HXX

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CheckableHeaderView.hxx"

#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

namespace OTGUI {
  
class OTGUI_API PhysicalModelWindowWidget : public QWidget
{
  Q_OBJECT

public :
  PhysicalModelWindowWidget(PhysicalModelItem * item);

protected:
  void buildInterface();

public slots:
  void addInputLine();
  void addOutputLine();
  void removeInputLine();
  void removeOutputLine();
  void evaluateOutputs();
  void updateInputTableModel();
  void updateOutputTableModel();

signals:
  void errorMessageChanged(QString);

private:
  PhysicalModel physicalModel_;
  CopyableTableView * inputTableView_;
  InputTableModel * inputTableModel_;
  QPushButton * addInputLineButton_;
  QPushButton * removeInputLineButton_;
  CopyableTableView * outputTableView_;
  CheckableHeaderView * outputTableHeaderView_;
  OutputTableModel * outputTableModel_;
  QPushButton * addOutputLineButton_;
  QPushButton * removeOutputLineButton_;
  QPushButton * evaluateOutputsButton_;
};
}
#endif