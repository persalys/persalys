//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for min/max table
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
#include "otgui/MinMaxTableGroupBox.hxx"

#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI
{

MinMaxTableGroupBox::MinMaxTableGroupBox(const DesignOfExperiment& doe, const bool onlyOutput, QWidget* parent)
  : QGroupBox(tr("Minimum and Maximum"), parent)
{
  QVBoxLayout * minMaxGroupBoxLayout = new QVBoxLayout(this);
  stackedWidget_ = new ResizableStackedWidget;

  // table for outputs
  if (doe.getOutputSample().getSize())
    for (UnsignedInteger outputIndex = 0; outputIndex < doe.getOutputSample().getDimension(); ++outputIndex)
      stackedWidget_->addWidget(getForOutputMinMaxTableView(doe, outputIndex));

  // table for inputs
  if (!onlyOutput && doe.getInputSample().getSize())
    for (UnsignedInteger inputIndex = 0; inputIndex < doe.getInputSample().getDimension(); ++inputIndex)
      stackedWidget_->addWidget(getForInputMinMaxTableView(doe, inputIndex));

  minMaxGroupBoxLayout->addWidget(stackedWidget_);
}


QWidget* MinMaxTableGroupBox::getForInputMinMaxTableView(const DesignOfExperiment& doe, const UnsignedInteger inputIndex)
{
  // table view
  ResizableTableViewWithoutScrollBar * minMaxTableView = new ResizableTableViewWithoutScrollBar;
  minMaxTableView->verticalHeader()->hide();
  // table model
  CustomStandardItemModel * minMaxTable = new CustomStandardItemModel(1, 4, minMaxTableView);
  minMaxTableView->setModel(minMaxTable);

  // horizontal header
  minMaxTable->setHorizontalHeaderLabels(QStringList() << tr("")
                                         << tr("Variable")
                                         << tr("Minimum")
                                         << tr("Maximum"));

  // vertical header
  minMaxTable->setNotEditableHeaderItem(0, 0, tr("Input"));
  // input name
  minMaxTable->setNotEditableItem(0, 1, QString::fromUtf8(doe.getInputSample().getDescription()[inputIndex].c_str()));
  // min
  minMaxTable->setNotEditableItem(0, 2, doe.getInputSample().getMin()[inputIndex]);
  // max
  minMaxTable->setNotEditableItem(0, 3, doe.getInputSample().getMax()[inputIndex]);

  // resize table
  minMaxTableView->resizeToContents();

  return minMaxTableView;
}


QWidget* MinMaxTableGroupBox::getForOutputMinMaxTableView(const DesignOfExperiment& doe, const UnsignedInteger outputIndex)
{
  // table view
  ResizableTableViewWithoutScrollBar * minMaxTableView = new ResizableTableViewWithoutScrollBar;
  minMaxTableView->verticalHeader()->hide();
  // table model
  const UnsignedInteger nbInputs = doe.getInputSample().getSize() > 0 ? doe.getInputSample().getDimension() : 0;
  CustomStandardItemModel * minMaxTable = new CustomStandardItemModel(nbInputs + 1, 4, minMaxTableView);
  minMaxTableView->setModel(minMaxTable);

  // horizontal header
  minMaxTable->setHorizontalHeaderLabels(QStringList() << tr("")
                                         << tr("Variable")
                                         << tr("Minimum")
                                         << tr("Maximum"));

  // vertical header
  minMaxTable->setNotEditableHeaderItem(0, 0, tr("Output"));

  // output name
  minMaxTable->setNotEditableItem(0, 1, QString::fromUtf8(doe.getOutputSample().getDescription()[outputIndex].c_str()));
  // min
  minMaxTable->setNotEditableItem(0, 2, doe.getOutputSample().getMin()[outputIndex]);
  // max
  minMaxTable->setNotEditableItem(0, 3, doe.getOutputSample().getMax()[outputIndex]);

  if (nbInputs)
  {
    // vertical header
    const QString rowTitle = tr("Inputs at\nextremum");
    minMaxTable->setNotEditableHeaderItem(1, 0, rowTitle);

    if (nbInputs > 1)
      minMaxTableView->setSpan(1, 0, nbInputs, 1);

    // inputs names
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
      minMaxTable->setNotEditableItem(i + 1, 1, QString::fromUtf8(doe.getInputSample().getDescription()[i].c_str()));

    // information message and warning icon
    if (doe.getImplementation()->getListXMin()[outputIndex].getSize() > 1)
    {
      minMaxTable->setHeaderData(2, Qt::Horizontal, QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      minMaxTable->setHeaderData(2, Qt::Horizontal, tr("Information: The output is minimum at another point."), Qt::ToolTipRole);
    }
    if (doe.getImplementation()->getListXMax()[outputIndex].getSize() > 1)
    {
      minMaxTable->setHeaderData(3, Qt::Horizontal, QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      minMaxTable->setHeaderData(3, Qt::Horizontal, tr("Information: The output is maximum at another point."), Qt::ToolTipRole);
    }
    // Xmin/XMax
    for (UnsignedInteger i = 0; i < doe.getInputSample().getDimension(); ++i)
    {
      // XMin
      minMaxTable->setNotEditableItem(i + 1, 2, doe.getImplementation()->getListXMin()[outputIndex](0, i));
      // XMax
      minMaxTable->setNotEditableItem(i + 1, 3, doe.getImplementation()->getListXMax()[outputIndex](0, i));
    }
  }

  // resize table
  minMaxTableView->resizeToContents();
  // 2nd row height correction because of the span
  if (nbInputs > 1)
  {
    const int h1 = minMaxTableView->verticalHeader()->sectionSize(2);
    const int h2 = minMaxTableView->verticalHeader()->sectionSize(1);
    minMaxTableView->verticalHeader()->resizeSection(1, minMaxTableView->verticalHeader()->sectionSize(2));
    minMaxTableView->setFixedSize(minMaxTableView->minimumSizeHint().width(), minMaxTableView->minimumSizeHint().height() - (h2 - h1));
  }

  return minMaxTableView;
}


void MinMaxTableGroupBox::setCurrentIndexStackedWidget(int index)
{
  stackedWidget_->setCurrentIndex(index);
}
}
