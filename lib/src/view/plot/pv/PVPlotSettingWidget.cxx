//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure paraview plot matrices
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/PVPlotSettingWidget.hxx"

#include "persalys/ListWidgetWithCheckBox.hxx"
#include "persalys/TitledComboBox.hxx"
#include "persalys/QtTools.hxx"

#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>

using namespace OT;

namespace PERSALYS
{

PVPlotSettingWidget::PVPlotSettingWidget(PVViewWidget *pvViewWidget, QWidget *parent)
  : QWidget(parent)
  , pvViewWidget_(pvViewWidget)
{
  if (!pvViewWidget_)
    throw InvalidArgumentException(HERE) << "PVPlotSettingWidget: No pvViewWidget !";

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  frameLayout_ = new QGridLayout(frame);
  frameLayout_->setColumnStretch(1, 2);

  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
}


void PVPlotSettingWidget::addSelectDataWidget(const QString &labelName, const QList<bool> &checked)
{
  Q_ASSERT(frameLayout_);

  int rowGrid = frameLayout_->rowCount();

  QLabel * label = new QLabel(labelName);
  frameLayout_->addWidget(label, rowGrid, 0);

  // combobox to select data to display
  TitledComboBox * comboBox = new TitledComboBox("-- " + tr("Select") + " --");

  QStringList reprNames;
  QStringList visibleReprNames;

  // representation
  if (pvViewWidget_->getNumberOfRepresentations() > 1)
  {
    for (int i = 0; i < pvViewWidget_->getNumberOfRepresentations(); ++i)
    {
      reprNames << pvViewWidget_->getRepresentationLabels(i)[0];
      if (pvViewWidget_->getRepresentationVisibility(i) && (!checked.size() || checked[i]))
        visibleReprNames << pvViewWidget_->getRepresentationLabels(i)[0];
    }
  }
  else
  {
    reprNames = plotNames_;
    for (int i=0; i<plotNames_.size(); ++i) {
      if (visibleReprNames.size() < MaxVisibleVariableNumber && (!checked.size() || checked[i]))
        visibleReprNames << plotNames_[i];}
  }
  ListWidgetWithCheckBox * reprListWidget = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", reprNames, visibleReprNames, this);
  if (pvViewWidget_->getNumberOfRepresentations() > 1)
    connect(reprListWidget, SIGNAL(checkedItemsChanged(QList<int>)), pvViewWidget_, SLOT(setRepresentationVisibility(QList<int>)));
  else
  {
    connect(reprListWidget, SIGNAL(checkedItemsChanged(QStringList)), pvViewWidget_, SLOT(setAxisToShow(QStringList)));
    if (plotNames_ != visibleReprNames) pvViewWidget_->setAxisToShow(visibleReprNames);
  }

  comboBox->setModel(reprListWidget->model());
  comboBox->setView(reprListWidget);

  frameLayout_->addWidget(comboBox, rowGrid, 1);
}


void PVPlotSettingWidget::addRankWidget(const bool checkState)
{
  QCheckBox * rankCheckBox = new QCheckBox(tr("Ranks"));
  rankCheckBox->setChecked(checkState);
  frameLayout_->addWidget(rankCheckBox, frameLayout_->rowCount(), 0, 1, 2);
  connect(rankCheckBox, &QCheckBox::clicked, [=](bool isRank) {
            for (UnsignedInteger i = 0; i < samples_.getSize(); ++i)
              pvViewWidget_->updateTable(isRank ? rankSamples_[i] : samples_[i], i);});
}


void PVPlotSettingWidget::addExportLayout()
{
  QHBoxLayout * exportLayout = new QHBoxLayout;
  QPushButton * button = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  connect(button, SIGNAL(clicked()), pvViewWidget_, SLOT(exportPlot()));
  exportLayout->addWidget(button);
  exportLayout->addStretch();

  frameLayout_->addLayout(exportLayout, frameLayout_->rowCount(), 0, 1, 2);
}


QSize PVPlotSettingWidget::sizeHint() const
{
  QSize size(QWidget::sizeHint());
  const int hScrollBarHeight = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  size.setHeight(size.height() + hScrollBarHeight);
  return size;
}


QSize PVPlotSettingWidget::minimumSizeHint() const
{
  QSize size = QWidget::minimumSizeHint();
  size.setHeight(10);
  return size;
}


// ------------- MultiPlotSettingWidget ------------- for parallel coordinates plot and plot matrix

MultiPlotSettingWidget::MultiPlotSettingWidget(PVViewWidget *pvViewWidget, const Sample &sample, const Sample &sampleRank, QWidget *parent)
  : PVPlotSettingWidget(pvViewWidget, parent)
{
  samples_ = Collection<Sample>(1, sample);
  rankSamples_ = Collection<Sample>(1, sampleRank);

  // add widgets
  QList<bool> checked;
  Description variablesNames = sample.getDescription();
  plotNames_ = QtOT::DescriptionToStringList(variablesNames);
  Point max = sample.getMax();
  Point min = sample.getMin();
  for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i) {
    checked << !(max[i] == min[i]);
  }

  addSelectDataWidget(tr("Variables"), checked);
  addRankWidget(true);
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);
}
}
