//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for Stopping criteria
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
#include "persalys/StopCriteriaGroupBox.hxx"

#include <openturns/ResourceMap.hxx>

#include <QGridLayout>
#include <QCheckBox>

#include <limits>

using namespace OT;

namespace PERSALYS
{

StopCriteriaGroupBox::StopCriteriaGroupBox(const Criteria criteria, QWidget* parent)
  : QGroupBox(tr("Stopping criteria"), parent)
  , criteria_(criteria)
  , maxiCoefOfVarCheckBox_(0)
  , maxiCoefficientOfVariationSpinbox_(0)
  , maxiCILengthCheckBox_(0)
  , maxiCILengthSpinbox_(0)
  , maxiTimeCheckBox_(0)
  , maxTimeLineEdit_(0)
  , maxiCallsCheckBox_(0)
  , maxiCallsSpinbox_(0)
{
  buildInterface();
}


void StopCriteriaGroupBox::buildInterface()
{
  QGridLayout * groupBoxLayout = new QGridLayout(this);

  if (criteria_ == Time_Calls_CoefVar)
  {
    // Maximum coefficient of variation
    maxiCoefOfVarCheckBox_ = new QCheckBox(tr("Accuracy - coefficient of variation"));
    maxiCoefficientOfVariationSpinbox_ = new DoubleSpinBox;
    maxiCoefficientOfVariationSpinbox_->setMinimum(0.);
    maxiCoefficientOfVariationSpinbox_->setMaximum(1.);
    maxiCoefficientOfVariationSpinbox_->setSingleStep(0.01);

    connect(maxiCoefficientOfVariationSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(criteriaChanged()));
    connect(maxiCoefOfVarCheckBox_, SIGNAL(toggled(bool)), maxiCoefficientOfVariationSpinbox_, SLOT(setEnabled(bool)));
    connect(maxiCoefOfVarCheckBox_, SIGNAL(toggled(bool)), this, SIGNAL(criteriaChanged()));

    groupBoxLayout->addWidget(maxiCoefOfVarCheckBox_, 0, 0);
    groupBoxLayout->addWidget(maxiCoefficientOfVariationSpinbox_, 0, 1);
  }
  else if (criteria_ == Time_Calls_CILength)
  {
    // Maximum confidence interval length
    maxiCILengthCheckBox_ = new QCheckBox(tr("Maximum confidence interval length"));
    maxiCILengthSpinbox_ = new DoubleSpinBox;
    maxiCILengthSpinbox_->setMinimum(0.);
    maxiCILengthSpinbox_->setMaximum(1.);
    maxiCILengthSpinbox_->setSingleStep(0.01);

    connect(maxiCILengthSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(criteriaChanged()));
    connect(maxiCILengthCheckBox_, SIGNAL(toggled(bool)), maxiCILengthSpinbox_, SLOT(setEnabled(bool)));
    connect(maxiCILengthCheckBox_, SIGNAL(toggled(bool)), this, SIGNAL(criteriaChanged()));

    groupBoxLayout->addWidget(maxiCILengthCheckBox_, 0, 0);
    groupBoxLayout->addWidget(maxiCILengthSpinbox_, 0, 1);
  }

  // Maximum time
  maxiTimeCheckBox_ = new QCheckBox(tr("Maximum time"));
  maxTimeLineEdit_ = new TimeLineEdit("");

  connect(maxTimeLineEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(criteriaChanged()));
  connect(maxiTimeCheckBox_, SIGNAL(toggled(bool)), maxTimeLineEdit_, SLOT(setEnabled(bool)));
  connect(maxiTimeCheckBox_, SIGNAL(toggled(bool)), this, SIGNAL(criteriaChanged()));

  groupBoxLayout->addWidget(maxiTimeCheckBox_, 1, 0);
  groupBoxLayout->addWidget(maxTimeLineEdit_, 1, 1);

  // Maximum function calls
  maxiCallsCheckBox_ = new QCheckBox(tr("Maximum calls"));
  maxiCallsSpinbox_ = new LogSpinBox;

  connect(maxiCallsSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(criteriaChanged()));
  connect(maxiCallsCheckBox_, SIGNAL(toggled(bool)), maxiCallsSpinbox_, SLOT(setEnabled(bool)));
  connect(maxiCallsCheckBox_, SIGNAL(toggled(bool)), this, SIGNAL(criteriaChanged()));

  groupBoxLayout->addWidget(maxiCallsCheckBox_, 2, 0);
  groupBoxLayout->addWidget(maxiCallsSpinbox_, 2, 1);
}


UnsignedInteger StopCriteriaGroupBox::getMaximumCalls() const
{
  if (maxiCallsCheckBox_->isChecked())
    return maxiCallsSpinbox_->value();
  return (UnsignedInteger)std::numeric_limits<int>::max();
}


void StopCriteriaGroupBox::setMaximumCalls(const UnsignedInteger maxCalls)
{
  maxiCallsSpinbox_->setValue(maxCalls < (UnsignedInteger)std::numeric_limits<int>::max() ? maxCalls : 10000);
  maxiCallsCheckBox_->setChecked(maxCalls < (UnsignedInteger)std::numeric_limits<int>::max());
  maxiCallsSpinbox_->setEnabled(maxCalls < (UnsignedInteger)std::numeric_limits<int>::max());
}


Scalar StopCriteriaGroupBox::getMaximumCoefficientOfVariation() const
{
  if (maxiCoefOfVarCheckBox_->isChecked())
    return maxiCoefficientOfVariationSpinbox_->value();
  return -1.0;
}


void StopCriteriaGroupBox::setMaximumCoefficientOfVariation(const Scalar maxCoef)
{
  maxiCoefficientOfVariationSpinbox_->setValue((maxCoef >= 0. && maxCoef <= 1.) ? maxCoef : 0.01);
  maxiCoefOfVarCheckBox_->setChecked(maxCoef >= 0. && maxCoef <= 1.);
  maxiCoefficientOfVariationSpinbox_->setEnabled(maxCoef >= 0. && maxCoef <= 1.);
}

Scalar StopCriteriaGroupBox::getMaximumConfidenceIntervalLength() const
{
  if (maxiCILengthCheckBox_->isChecked())
    return maxiCILengthSpinbox_->value();
  return -1.0;
}


void StopCriteriaGroupBox::setMaximumConfidenceIntervalLength(const Scalar length)
{
  const Scalar defaultValue = ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon");
  maxiCILengthSpinbox_->setValue(length <= 0. ? defaultValue : length);
  maxiCILengthCheckBox_->setChecked(length > 0.);
  maxiCILengthSpinbox_->setEnabled(length > 0.);
}


Scalar StopCriteriaGroupBox::getMaximumElapsedTime() const
{
  if (maxiTimeCheckBox_->isChecked())
    return maxTimeLineEdit_->getSeconds();
  return -1;
}


void StopCriteriaGroupBox::setMaximumElapsedTime(const Scalar maxTime)
{
  bool validMaxTime = (int)maxTime < std::numeric_limits<int>::max() && maxTime > 0;
  maxTimeLineEdit_->setSeconds(validMaxTime ? maxTime : 60);
  maxiTimeCheckBox_->setChecked(validMaxTime);
  maxTimeLineEdit_->setEnabled(validMaxTime);
}


bool StopCriteriaGroupBox::isMaxElapsedTimeValid() const
{
  return !(maxTimeLineEdit_->isEnabled() && !maxTimeLineEdit_->getSeconds());
}


bool StopCriteriaGroupBox::isMaxCallsRequired() const
{
  return maxiCallsSpinbox_->isEnabled();
}


bool StopCriteriaGroupBox::isValid() const
{
  if (criteria_ == Time_Calls)
    return (maxTimeLineEdit_->isEnabled() ||
            maxiCallsSpinbox_->isEnabled());
  else if (criteria_ == Time_Calls_CoefVar)
    return (maxiCoefficientOfVariationSpinbox_->isEnabled() ||
            maxTimeLineEdit_->isEnabled() ||
            maxiCallsSpinbox_->isEnabled());
  else if (criteria_ == Time_Calls_CILength)
    return (maxiCILengthSpinbox_->isEnabled() ||
            maxTimeLineEdit_->isEnabled() ||
            maxiCallsSpinbox_->isEnabled());
  return false;
}
}
