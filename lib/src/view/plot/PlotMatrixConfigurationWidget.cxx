//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure plot matrices
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
#include "otgui/PlotMatrixConfigurationWidget.hxx"

#include "otgui/ListWidgetWithCheckBox.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/TitledComboBox.hxx"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>

namespace OTGUI
{

PlotMatrixConfigurationWidget::PlotMatrixConfigurationWidget(PlotMatrixWidget * plotMatrix, QWidget* parent)
  : QWidget(parent)
  , plotMatrix_(plotMatrix)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QGridLayout * mainGridLayout = new QGridLayout(frame);
  int rowGrid = 0;

  // title
  QLabel * label = new QLabel(tr("Title"));
  mainGridLayout->addWidget(label, rowGrid, 0, 1, 1);
  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  mainGridLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  // show Outputs vs Inputs
  if (plotMatrix_->getOutputNames().size())
  {
    QCheckBox * outputVsInputCheckBox = new QCheckBox(tr("Outputs vs inputs"));
    outputVsInputCheckBox->setChecked(false);
    connect(outputVsInputCheckBox, SIGNAL(clicked(bool)), this, SLOT(showXY(bool)));
    mainGridLayout->addWidget(outputVsInputCheckBox, ++rowGrid, 0, 1, 2);
  }

  // columns label
  label = new QLabel(tr("Columns"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  // combobox to select the columns to display
  TitledComboBox * inputsComboBox = new TitledComboBox("-- " + tr("Select variables") + " --");
  columnsListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select variables") + " --", plotMatrix_->getColumnsNames(), this);
  connect(columnsListWidget_, SIGNAL(checkedItemsChanged(QStringList)), plotMatrix_, SLOT(setColumnsToDisplay(QStringList)));
  inputsComboBox->setModel(columnsListWidget_->model());
  inputsComboBox->setView(columnsListWidget_);
  mainGridLayout->addWidget(inputsComboBox, rowGrid, 1, 1, 1);

  // rows label
  label = new QLabel(tr("Rows"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  // combobox to select the rows to display
  TitledComboBox * outputsComboBox = new TitledComboBox("-- " + tr("Select variables") + " --");
  rowsListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select variables") + " --", plotMatrix_->getRowsNames(), this);
  connect(rowsListWidget_, SIGNAL(checkedItemsChanged(QStringList)), plotMatrix_, SLOT(setRowsToDisplay(QStringList)));
  outputsComboBox->setModel(rowsListWidget_->model());
  outputsComboBox->setView(rowsListWidget_);
  mainGridLayout->addWidget(outputsComboBox, rowGrid, 1, 1, 1);

  // pushbutton to export the plot
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);

  mainGridLayout->addLayout(hboxForBottomButtons, ++rowGrid, 1, 1, 1);

  mainGridLayout->setRowStretch(++rowGrid, 1);

  updateLineEdits();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
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


void PlotMatrixConfigurationWidget::showXY(bool isChecked)
{
  if (!isChecked)
  {
    rowsListWidget_->setCheckedNames(plotMatrix_->getRowsNames());
    columnsListWidget_->setCheckedNames(plotMatrix_->getColumnsNames());
  }
  else
  {
    rowsListWidget_->setCheckedNames(plotMatrix_->getOutputNames());
    columnsListWidget_->setCheckedNames(plotMatrix_->getInputNames());
  }
}


void PlotMatrixConfigurationWidget::exportPlot()
{
  plotMatrix_->exportPlot();
}


QSize PlotMatrixConfigurationWidget::sizeHint() const
{
  QSize size = QWidget::sizeHint();
  const int hScrollBarHeight = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  size.setHeight(size.height() + hScrollBarHeight);
  return size;
}


QSize PlotMatrixConfigurationWidget::minimumSizeHint() const
{
  QSize size = QWidget::minimumSizeHint();
  size.setHeight(10);
  return size;
}
}
