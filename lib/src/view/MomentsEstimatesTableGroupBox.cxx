//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for moments estimates table
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/MomentsEstimatesTableGroupBox.hxx"

#include "persalys/CopyableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace PERSALYS
{

MomentsEstimatesTableGroupBox::MomentsEstimatesTableGroupBox(const DataAnalysisResult& result,
    const bool isConfidenceIntervalRequired,
    const double levelConfidenceInterval,
    const Indices& variablesIndices,
    QWidget* parent)
  : QGroupBox(tr("Moments estimates"), parent)
  , result_(result)
  , isConfidenceIntervalRequired_(isConfidenceIntervalRequired)
  , levelConfidenceInterval_(levelConfidenceInterval)
  , variablesIndices_(variablesIndices)
{
  if (!variablesIndices.check(result_.getMean().getSize()))
    throw InvalidArgumentException(HERE) << "The result dimension does not match the sample dimension";

  QVBoxLayout * estimatesGroupBoxLayout = new QVBoxLayout(this);
  stackedWidget_ = new ResizableStackedWidget;

  for (UnsignedInteger i = 0; i < variablesIndices.getSize(); ++i)
    stackedWidget_->addWidget(getMomentsEstimateTableView(variablesIndices[i]));

  estimatesGroupBoxLayout->addWidget(stackedWidget_);

  // quantiles
  QGridLayout * quantLayout = new QGridLayout;

  // Probability
  QLabel * label = new QLabel(tr("Probability"));
  quantLayout->addWidget(label, 0, 0);

  probaSpinBox_ = new DoubleSpinBox;
  label->setBuddy(probaSpinBox_);
  probaSpinBox_->setMinimum(0.0);
  probaSpinBox_->setMaximum(1.0);
  probaSpinBox_->setSingleStep(0.01);
  quantLayout->addWidget(probaSpinBox_, 0, 1);

  // Quantile
  label = new QLabel(tr("Empirical quantile"));
  quantLayout->addWidget(label, 0, 2);

  quantileSpinBox_ = new DoubleSpinBox;
  label->setBuddy(quantileSpinBox_);
  quantileSpinBox_->setDecimals(8);
  quantLayout->addWidget(quantileSpinBox_, 0, 3);

  quantLayout->setColumnStretch(1, 2);
  quantLayout->setColumnStretch(3, 2);

  connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
  connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));

  estimatesGroupBoxLayout->addLayout(quantLayout);
  updateSpinBoxes();
}


QWidget* MomentsEstimatesTableGroupBox::getMomentsEstimateTableView(const UnsignedInteger variableIndex)
{
  // table view
  CopyableTableView * momentsEstimationsTableView = new CopyableTableView;
  momentsEstimationsTableView->horizontalHeader()->hide();
  momentsEstimationsTableView->verticalHeader()->hide();

  // table model
  const int nbColumns = isConfidenceIntervalRequired_ ? 3 : 2;
  const int nbRows = 8;
  CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(nbRows, nbColumns, momentsEstimationsTableView);
  momentsEstimationsTableView->setModel(momentsEstimationsTable);

  // vertical header
  int row = 0;
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Mean"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Standard deviation"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Coefficient of variation"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Skewness"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Kurtosis"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("First quartile"));
  momentsEstimationsTable->setNotEditableHeaderItem(++row, 0, tr("Third quartile"));

  // horizontal header
  row = 0;
  momentsEstimationsTable->setNotEditableHeaderItem(row, 0, tr("Estimate"));
  momentsEstimationsTable->setNotEditableHeaderItem(row, 1, tr("Value"));
  if (isConfidenceIntervalRequired_)
    momentsEstimationsTable->setNotEditableHeaderItem(0, 2, tr("Confidence interval\nat") + " " + QString::number(levelConfidenceInterval_ * 100) + "%");

  // moments values

  // Mean
  momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getMean()[variableIndex][0]);

  if (isConfidenceIntervalRequired_)
  {
    if (result_.getMeanConfidenceInterval().getFiniteLowerBound()[variableIndex])
      momentsEstimationsTable->setNotEditableItem(row, 2, result_.getMeanConfidenceInterval().getMarginal(variableIndex).__str__().c_str());
    else
      momentsEstimationsTable->setNotEditableItem(row, 2, "-");
  }

  // Standard Deviation
  if (result_.getStandardDeviation()[variableIndex].getSize() > 0)
    momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getStandardDeviation()[variableIndex][0]);
  else
    momentsEstimationsTable->setNotEditableItem(++row, 1, "-");

  if (isConfidenceIntervalRequired_)
  {
    if (result_.getStdConfidenceInterval().getFiniteLowerBound()[variableIndex])
      momentsEstimationsTable->setNotEditableItem(row, 2, result_.getStdConfidenceInterval().getMarginal(variableIndex).__str__().c_str());
    else
      momentsEstimationsTable->setNotEditableItem(row, 2, "-");
  }

  // Coefficient of variation
  if (result_.getCoefficientOfVariation()[variableIndex].getSize() > 0)
    momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getCoefficientOfVariation()[variableIndex][0]);
  else
    momentsEstimationsTable->setNotEditableItem(++row, 1, "-");
  // Skewness
  if (result_.getSkewness()[variableIndex].getSize() > 0)
    momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getSkewness()[variableIndex][0]);
  else
    momentsEstimationsTable->setNotEditableItem(++row, 1, "-");
  // Kurtosis
  if (result_.getKurtosis()[variableIndex].getSize() > 0)
    momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getKurtosis()[variableIndex][0]);
  else
    momentsEstimationsTable->setNotEditableItem(++row, 1, "-");
  // First quartile
  if (result_.getFirstQuartile()[variableIndex].getSize() > 0)
    momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getFirstQuartile()[variableIndex][0]);
  // Third quartile
  if (result_.getThirdQuartile()[variableIndex].getSize() > 0)
    momentsEstimationsTable->setNotEditableItem(++row, 1, result_.getThirdQuartile()[variableIndex][0]);

  // resize table
  momentsEstimationsTableView->resizeToContents();

  return momentsEstimationsTableView;
}


void MomentsEstimatesTableGroupBox::updateSpinBoxes()
{
  if (!quantileSpinBox_)
    return;

  SignalBlocker blocker(quantileSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesIndices_[stackedWidget_->currentIndex()];

  // check
  if (indexVar >= result_.getMin().getSize() || indexVar >= result_.getMax().getSize())
    throw InvalidArgumentException(HERE) << "The result dimension does not match the sample dimension";

  if (result_.getMin().getSize() && result_.getMax().getSize())
  {
    const double min = result_.getMin()[indexVar][0];
    const double max = result_.getMax()[indexVar][0];

    quantileSpinBox_->setMinimum(min);
    quantileSpinBox_->setMaximum(max);
    quantileSpinBox_->setSingleStep((max - min) / 100);
  }
  probaSpinBox_->setValue(0.5);
  // if the previous value of probaSpinBox_ was 0.5, the signal valueChanged is not emitted
  probaValueChanged(0.5);
}


void MomentsEstimatesTableGroupBox::probaValueChanged(double proba)
{
  SignalBlocker blocker(quantileSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesIndices_[stackedWidget_->currentIndex()];
  quantileSpinBox_->setValue(result_.getDesignOfExperiment().getSample().getMarginal(indexVar).computeQuantile(proba)[0]);
}


void MomentsEstimatesTableGroupBox::quantileValueChanged(double quantile)
{
  SignalBlocker blocker(probaSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesIndices_[stackedWidget_->currentIndex()];

  double cdf = 0.0;
  const double p = 1.0 / double(result_.getDesignOfExperiment().getSample().getSize());

  for (UnsignedInteger j = 0; j < result_.getDesignOfExperiment().getSample().getSize(); ++j)
    if (result_.getDesignOfExperiment().getSample()[j][indexVar] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
}


void MomentsEstimatesTableGroupBox::setCurrentIndexStackedWidget(int index)
{
  stackedWidget_->setCurrentIndex(index);
  updateSpinBoxes();
}
}
