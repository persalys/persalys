//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for moments estimates table
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
#include "otgui/MomentsEstimatesTableGroupBox.hxx"

#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

MomentsEstimatesTableGroupBox::MomentsEstimatesTableGroupBox(const DataAnalysisResult & result,
                                                             const bool isConfidenceIntervalRequired,
                                                             const double levelConfidenceInterval,
                                                             QWidget* parent)
  : QGroupBox(tr("Moments estimates"), parent)
  , isConfidenceIntervalRequired_(isConfidenceIntervalRequired)
  , levelConfidenceInterval_(levelConfidenceInterval)
{
  if (result.isValid())
  {
    QVBoxLayout * estimatesGroupBoxLayout = new QVBoxLayout(this);
    stackedWidget_ = new QStackedWidget;

    for (UnsignedInteger variableIndex=0; variableIndex<result.getMean().getSize(); ++variableIndex)
      stackedWidget_->addWidget(getMomentsEstimateTableView(result, variableIndex));

    estimatesGroupBoxLayout->addWidget(stackedWidget_);
  }
  else
  {
    throw InvalidArgumentException(HERE) << "Can not build MomentsEstimatesTableGroupBox: the result is invalid";
  }
}


QWidget* MomentsEstimatesTableGroupBox::getMomentsEstimateTableView(const DataAnalysisResult & result, const int variableIndex)
{
  const int nbColumns = isConfidenceIntervalRequired_ ? 4 : 2;
  const int nbRows = isConfidenceIntervalRequired_ ? 9 : 8;
  const bool meanIntervalRequired = isConfidenceIntervalRequired_ &&
                                    result.getMeanConfidenceInterval().getDimension() == result.getMean().getSize();

  const bool stdIntervalRequired = isConfidenceIntervalRequired_ &&
                                   result.getStdConfidenceInterval().getDimension() == result.getMean().getSize();

  ResizableTableViewWithoutScrollBar * momentsEstimationsTableView = new ResizableTableViewWithoutScrollBar;
  momentsEstimationsTableView->horizontalHeader()->hide();
  momentsEstimationsTableView->verticalHeader()->hide();
  CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(nbRows, nbColumns);
  momentsEstimationsTableView->setModel(momentsEstimationsTable);

  // vertical header
  int row = 0;
  momentsEstimationsTable->setNotEditableHeaderItem(row, 0, tr("Estimate"));
  if (isConfidenceIntervalRequired_)
  {
    momentsEstimationsTableView->setSpan(row, 0, 2, 1);
    ++row;
  }
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Mean"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Standard deviation"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Coefficient of variation"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Skewness"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Kurtosis"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("First quartile"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Third quartile"));

  // horizontal header
  row = 0;
  momentsEstimationsTable->setNotEditableHeaderItem(row, 1, tr("Value"));
  if (isConfidenceIntervalRequired_)
  {
    momentsEstimationsTableView->setSpan(row, 1, 2, 1);
    momentsEstimationsTable->setNotEditableHeaderItem(++row, 2, tr("Lower bound"));
    momentsEstimationsTable->setNotEditableHeaderItem(row, 3, tr("Upper bound"));
  }
  // Mean
  momentsEstimationsTable->setNotEditableItem(++row, 1, result.getMean()[variableIndex]);

  if (meanIntervalRequired)
  {
    const double meanCILowerBound = result.getMeanConfidenceInterval().getLowerBound()[variableIndex];
    momentsEstimationsTable->setNotEditableItem(row, 2, meanCILowerBound);
    const double meanCIUpperBound = result.getMeanConfidenceInterval().getUpperBound()[variableIndex];
    momentsEstimationsTable->setNotEditableItem(row, 3, meanCIUpperBound);
  }
  // Standard Deviation
  momentsEstimationsTable->setNotEditableItem(++row, 1, result.getStandardDeviation()[variableIndex]);

  if (stdIntervalRequired)
  {
    const double stdCILowerBound = result.getStdConfidenceInterval().getLowerBound()[variableIndex];
    momentsEstimationsTable->setNotEditableItem(row, 2, stdCILowerBound);
    const double stdCIUpperBound = result.getStdConfidenceInterval().getUpperBound()[variableIndex];
    momentsEstimationsTable->setNotEditableItem(row, 3, stdCIUpperBound);
  }
  // Coefficient of variation
  momentsEstimationsTable->setNotEditableItem(++row, 1, result.getCoefficientOfVariation()[variableIndex]);
  // Skewness
  if (result.getSkewness()[variableIndex] == std::numeric_limits<double>::max())
    momentsEstimationsTable->setNotEditableItem(++row, 1, "-");
  else
    momentsEstimationsTable->setNotEditableItem(++row, 1, result.getSkewness()[variableIndex]);
  // Kurtosis
  if (result.getKurtosis()[variableIndex] == std::numeric_limits<double>::max())
    momentsEstimationsTable->setNotEditableItem(++row, 1, "-");
  else
    momentsEstimationsTable->setNotEditableItem(++row, 1, result.getKurtosis()[variableIndex]);
  // First quartile
  momentsEstimationsTable->setNotEditableItem(++row, 1, result.getFirstQuartile()[variableIndex]);
  // Third quartile
  momentsEstimationsTable->setNotEditableItem(++row, 1, result.getThirdQuartile()[variableIndex]);

  // resize table
  int titleWidth = 0;
  if (isConfidenceIntervalRequired_)
  {
    momentsEstimationsTable->setNotEditableHeaderItem(0, 2, tr("Confidence interval at") + " " + QString::number(levelConfidenceInterval_*100) + "%");
    momentsEstimationsTableView->resizeColumnsToContents();
    titleWidth = momentsEstimationsTableView->horizontalHeader()->sectionSize(2);

    // first: clear item at (0,2) because the text is to wide:
    // resizeColumnsToContents takes into account the text of item at (0,2)
    // to resize the column 2, even if there is a setSpan(0, 2, 1, 2)
    momentsEstimationsTable->setItem(0, 2, new QStandardItem);
  }

  momentsEstimationsTableView->resizeToContents();

  if (isConfidenceIntervalRequired_)
  {
    momentsEstimationsTable->setNotEditableHeaderItem(0, 2, tr("Confidence interval at") + " " + QString::number(levelConfidenceInterval_*100) + "%");
    momentsEstimationsTableView->setSpan(0, 2, 1, 2);
    const int subTitlesWidth = momentsEstimationsTableView->horizontalHeader()->sectionSize(2) + momentsEstimationsTableView->horizontalHeader()->sectionSize(3);
    const int widthCorrection = titleWidth - subTitlesWidth;
    if (widthCorrection > 0)
    {
      // correct the table width
      momentsEstimationsTableView->horizontalHeader()->resizeSection(3, momentsEstimationsTableView->horizontalHeader()->sectionSize(3) + widthCorrection);
      momentsEstimationsTableView->setMinimumWidth(momentsEstimationsTableView->minimumWidth() + widthCorrection);
    }
  }
  return momentsEstimationsTableView;
}


void MomentsEstimatesTableGroupBox::setCurrentIndexStackedWidget(int index)
{
  stackedWidget_->setCurrentIndex(index);
}
}