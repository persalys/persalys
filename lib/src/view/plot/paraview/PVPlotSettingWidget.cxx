//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure paraview plot matrices
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/PVPlotSettingWidget.hxx"

#include "otgui/ListWidgetWithCheckBox.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/NoWheelEventComboBox.hxx"

#include <vtkSMProperty.h>
#include <pqSMAdaptor.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QLabel>

namespace OTGUI {

PVPlotSettingWidget::PVPlotSettingWidget(PVViewWidget * pvViewWidget, QWidget* parent)
  : QWidget(parent)
  , pvViewWidget_(pvViewWidget)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QGridLayout * mainGridLayout = new QGridLayout(frame);
  int rowGrid = 0;

  // title ? can not :(

  // variables names
  QStringList names;
  for (int cc = 0; cc < (int)pvViewWidget->getTable()->GetNumberOfColumns(); cc++)
  {
    names << pvViewWidget->getTable()->GetColumnName(cc);
  }

  // variables to display
  QLabel * label = new QLabel(tr("Variables"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  // combobox to select the variables to display
  NoWheelEventComboBox * varComboBox = new NoWheelEventComboBox;
  ListWidgetWithCheckBox * varListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", names, names, this);
  connect(varListWidget_, SIGNAL(checkedItemsChanged(QStringList)), pvViewWidget_, SLOT(setAxisToShow(QStringList)));
  varComboBox->setModel(varListWidget_->model());
  varComboBox->setView(varListWidget_);
  mainGridLayout->addWidget(varComboBox, rowGrid, 1, 1, 1);

  // pushbutton to export the plot
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);

  mainGridLayout->addLayout(hboxForBottomButtons, ++rowGrid, 1, 1, 1);

  mainGridLayout->setRowStretch(++rowGrid, 1);

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
}


void PVPlotSettingWidget::exportPlot()
{
  pvViewWidget_->exportPlot();
}
}
