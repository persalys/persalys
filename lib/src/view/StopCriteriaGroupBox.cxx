//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for stop criteria
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
#include "otgui/StopCriteriaGroupBox.hxx"

#include <QGridLayout>
#include <QCheckBox>

#include <limits>

using namespace OT;

namespace OTGUI
{

StopCriteriaGroupBox::StopCriteriaGroupBox(QWidget* parent)
  : QGroupBox(tr("Stop criteria"), parent)
{
  buildInterface();
}


void StopCriteriaGroupBox::buildInterface()
{
  QGridLayout * groupBoxLayout = new QGridLayout(this);

  // Maximum coefficient of variation
  maxiCoefOfVarCheckBox_ = new QCheckBox(tr("Accuracy - coefficient of variation"));
  maxiCoefficientOfVariationSpinbox_ = new DoubleSpinBox;
  maxiCoefficientOfVariationSpinbox_->setMinimum(0.);
  maxiCoefficientOfVariationSpinbox_->setMaximum(1.);
  maxiCoefficientOfVariationSpinbox_->setSingleStep(0.01);

  connect(maxiCoefficientOfVariationSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(maxiCoefficientOfVariationChanged(double)));
  connect(maxiCoefOfVarCheckBox_, SIGNAL(toggled(bool)), maxiCoefficientOfVariationSpinbox_, SLOT(setEnabled(bool)));
  connect(maxiCoefOfVarCheckBox_, SIGNAL(toggled(bool)), this, SLOT(maxiCoefficientOfVariationRequired(bool)));

  groupBoxLayout->addWidget(maxiCoefOfVarCheckBox_, 0, 0);
  groupBoxLayout->addWidget(maxiCoefficientOfVariationSpinbox_, 0, 1);

  // Maximum time
  maxiTimeCheckBox_ = new QCheckBox(tr("Maximum time"));
  maxTimeLineEdit_ = new TimeLineEdit("");

  connect(maxTimeLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(maxiTimeChanged()));
  connect(maxiTimeCheckBox_, SIGNAL(toggled(bool)), maxTimeLineEdit_, SLOT(setEnabled(bool)));
  connect(maxiTimeCheckBox_, SIGNAL(toggled(bool)), this, SLOT(maxiTimeRequired(bool)));

  groupBoxLayout->addWidget(maxiTimeCheckBox_, 1, 0);
  groupBoxLayout->addWidget(maxTimeLineEdit_, 1, 1);

  // Maximum function calls
  maxiCallsCheckBox_ = new QCheckBox(tr("Maximum calls"));
  maxiCallsSpinbox_ = new LogSpinBox;

  connect(maxiCallsSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(maxiCallsChanged(double)));
  connect(maxiCallsCheckBox_, SIGNAL(toggled(bool)), maxiCallsSpinbox_, SLOT(setEnabled(bool)));
  connect(maxiCallsCheckBox_, SIGNAL(toggled(bool)), this, SLOT(maxiCallsRequired(bool)));

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


double StopCriteriaGroupBox::getMaximumCoefficientOfVariation() const
{
  if (maxiCoefOfVarCheckBox_->isChecked())
    return maxiCoefficientOfVariationSpinbox_->value();
  return -1.0;
}


void StopCriteriaGroupBox::setMaximumCoefficientOfVariation(const double maxCoef)
{
  maxiCoefficientOfVariationSpinbox_->setValue((maxCoef >= 0. && maxCoef <= 1.) ? maxCoef : 0.01);
  maxiCoefOfVarCheckBox_->setChecked(maxCoef >= 0. && maxCoef <= 1.);
  maxiCoefficientOfVariationSpinbox_->setEnabled(maxCoef >= 0. && maxCoef <= 1.);
}


UnsignedInteger StopCriteriaGroupBox::getMaximumElapsedTime() const
{
  if (maxiTimeCheckBox_->isChecked())
    return maxTimeLineEdit_->getSeconds();
  return (UnsignedInteger)std::numeric_limits<int>::max();
}


void StopCriteriaGroupBox::setMaximumElapsedTime(const UnsignedInteger maxTime)
{
  maxTimeLineEdit_->setSeconds(maxTime < (UnsignedInteger)std::numeric_limits<int>::max() ? maxTime : 60);
  maxiTimeCheckBox_->setChecked(maxTime < (UnsignedInteger)std::numeric_limits<int>::max());
  maxTimeLineEdit_->setEnabled(maxTime < (UnsignedInteger)std::numeric_limits<int>::max());
}


bool StopCriteriaGroupBox::isMaxElapsedTimeValid() const
{
  return !(maxTimeLineEdit_->isEnabled() && !maxTimeLineEdit_->getSeconds());
}


bool StopCriteriaGroupBox::isMaxCallsRequired() const
{
  return maxiCallsSpinbox_->isEnabled();
}


void StopCriteriaGroupBox::maxiCoefficientOfVariationRequired(bool checkboxClicked)
{
  double value = checkboxClicked ? maxiCoefficientOfVariationSpinbox_->value() : -1.;
  emit maxiCoefficientOfVariationChanged(value);
}


void StopCriteriaGroupBox::maxiCallsRequired(bool checkboxClicked)
{
  const int value = checkboxClicked ? maxiCallsSpinbox_->value() : std::numeric_limits<int>::max();
  emit maxiCallsChanged(value);
}


void StopCriteriaGroupBox::maxiTimeChanged()
{
  const int value = maxTimeLineEdit_->getSeconds() ? maxTimeLineEdit_->getSeconds() : std::numeric_limits<int>::max();
  emit maxiTimeChanged(value);
}


void StopCriteriaGroupBox::maxiTimeRequired(bool checkboxClicked)
{
  if (!maxTimeLineEdit_->getSeconds())
    maxTimeLineEdit_->setText("dh1ms");
  const int value = checkboxClicked ? maxTimeLineEdit_->getSeconds() : std::numeric_limits<int>::max();
  emit maxiTimeChanged(value);
}


bool StopCriteriaGroupBox::isValid() const
{
  return (maxiCoefficientOfVariationSpinbox_->isEnabled() ||
          maxTimeLineEdit_->isEnabled() ||
          maxiCallsSpinbox_->isEnabled());
}
}