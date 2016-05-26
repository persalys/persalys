//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure plot matrices
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
#include "otgui/PlotMatrixConfigurationWidget.hxx"

#include "otgui/ListWidgetWithCheckBox.hxx"
#include "otgui/QtTools.hxx"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>

namespace OTGUI {

PlotMatrixConfigurationWidget::PlotMatrixConfigurationWidget(PlotMatrixWidget * plotMatrix)
  : QWidget()
  , plotMatrix_(plotMatrix)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QGridLayout * mainGridLayout = new QGridLayout(frame);
  int rowGrid = 0;

  QLabel * label = new QLabel(tr("Title"));
  mainGridLayout->addWidget(label, rowGrid, 0, 1, 1);
  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  mainGridLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  label = new QLabel(tr("Colums"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  QComboBox * inputsComboBox = new QComboBox;
  ListWidgetWithCheckBox * listWidget = new ListWidgetWithCheckBox("-- Select variables --", plotMatrix_->getInputNames());
  connect(listWidget, SIGNAL(checkedItemsChanged(QStringList)), plotMatrix_, SLOT(setInputsToDisplay(QStringList)));
  inputsComboBox->setModel(listWidget->model());
  inputsComboBox->setView(listWidget);
  mainGridLayout->addWidget(inputsComboBox, rowGrid, 1, 1, 1);

  label = new QLabel(tr("Rows"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  QComboBox * outputsComboBox = new QComboBox;
  listWidget = new ListWidgetWithCheckBox("-- Select variables --", plotMatrix_->getOutputNames());
  connect(listWidget, SIGNAL(checkedItemsChanged(QStringList)), plotMatrix_, SLOT(setOutputsToDisplay(QStringList)));
  outputsComboBox->setModel(listWidget->model());
  outputsComboBox->setView(listWidget);
  mainGridLayout->addWidget(outputsComboBox, rowGrid, 1, 1, 1);

  // Bottom layout
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);

  mainGridLayout->addLayout(hboxForBottomButtons, ++rowGrid, 1, 1, 1);

  mainGridLayout->setRowStretch(++rowGrid, 1);

  updateLineEdits();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
}


void PlotMatrixConfigurationWidget::updateLineEdits()
{
  SignalBlocker blocker(titleLineEdit_);
  titleLineEdit_->setText(plotMatrix_->getTitle());
}


void PlotMatrixConfigurationWidget::updateTitle()
{
  plotMatrix_->setTitle(titleLineEdit_->text());
}


void PlotMatrixConfigurationWidget::exportPlot()
{
  plotMatrix_->exportPlot();
}
}