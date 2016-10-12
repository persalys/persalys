//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for stop criteria
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
#include "otgui/StopCriteriaGroupBox.hxx"

#include <QGridLayout>
#include <QCheckBox>

#include <limits>

using namespace OT;

namespace OTGUI {

StopCriteriaGroupBox::StopCriteriaGroupBox(const double maxCoef, const UnsignedInteger maxTime, const UnsignedInteger maxCalls, QWidget* parent)
  : QGroupBox(tr("Stop criteria"), parent)
{
  QGridLayout * groupBoxLayout = new QGridLayout(this);

  // Maximum coefficient of variation
  QCheckBox * maxiCoefOfVarCheckBox = new QCheckBox(tr("Accuracy - coefficient of variation"));
  maxiCoefficientOfVariationSpinbox_ = new DoubleSpinBox;
  maxiCoefficientOfVariationSpinbox_->setMinimum(0.);
  maxiCoefficientOfVariationSpinbox_->setMaximum(1.);
  maxiCoefficientOfVariationSpinbox_->setSingleStep(0.01);
  const double maxCoefOfVar = maxCoef;
  maxiCoefficientOfVariationSpinbox_->setValue((maxCoef >= 0. && maxCoef <= 1.) ? maxCoef : 0.01);

  maxiCoefOfVarCheckBox->setChecked(maxCoefOfVar >= 0. && maxCoefOfVar <= 1.);
  maxiCoefficientOfVariationSpinbox_->setEnabled(maxCoefOfVar >= 0. && maxCoefOfVar <= 1.);

  connect(maxiCoefficientOfVariationSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(maxiCoefficientOfVariationChanged(double)));
  connect(maxiCoefOfVarCheckBox, SIGNAL(toggled(bool)), maxiCoefficientOfVariationSpinbox_, SLOT(setEnabled(bool)));
  connect(maxiCoefOfVarCheckBox, SIGNAL(toggled(bool)), this, SLOT(maxiCoefficientOfVariationRequired(bool)));

  groupBoxLayout->addWidget(maxiCoefOfVarCheckBox, 0, 0);
  groupBoxLayout->addWidget(maxiCoefficientOfVariationSpinbox_, 0, 1);

  // Maximum time
  QCheckBox * maxiTimeCheckBox = new QCheckBox(tr("Maximum time"));
  maxTimeLineEdit_ = new TimeLineEdit(maxTime < (UnsignedInteger)std::numeric_limits<int>::max()? maxTime : 60);

  maxiTimeCheckBox->setChecked(maxTime < (UnsignedInteger)std::numeric_limits<int>::max());
  maxTimeLineEdit_->setEnabled(maxTime < (UnsignedInteger)std::numeric_limits<int>::max());

  connect(maxTimeLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(maxiTimeChanged()));
  connect(maxiTimeCheckBox, SIGNAL(toggled(bool)), maxTimeLineEdit_, SLOT(setEnabled(bool)));
  connect(maxiTimeCheckBox, SIGNAL(toggled(bool)), this, SLOT(maxiTimeRequired(bool)));

  groupBoxLayout->addWidget(maxiTimeCheckBox, 1, 0);
  groupBoxLayout->addWidget(maxTimeLineEdit_, 1, 1);

  // Maximum function calls
  QCheckBox * maxiCallsCheckBox = new QCheckBox(tr("Maximum calls"));
  maxiCallsSpinbox_ = new QSpinBox;
  maxiCallsSpinbox_->setMinimum(2);
  maxiCallsSpinbox_->setMaximum(std::numeric_limits<int>::max());
  maxiCallsSpinbox_->setValue(maxCalls < (UnsignedInteger)std::numeric_limits<int>::max()? maxCalls : 10000);

  maxiCallsCheckBox->setChecked(maxCalls < (UnsignedInteger)std::numeric_limits<int>::max());
  maxiCallsSpinbox_->setEnabled(maxCalls < (UnsignedInteger)std::numeric_limits<int>::max());

  connect(maxiCallsSpinbox_, SIGNAL(valueChanged(int)), this, SIGNAL(maxiCallsChanged(int)));
  connect(maxiCallsCheckBox, SIGNAL(toggled(bool)), maxiCallsSpinbox_, SLOT(setEnabled(bool)));
  connect(maxiCallsCheckBox, SIGNAL(toggled(bool)), this, SLOT(maxiCallsRequired(bool)));

  groupBoxLayout->addWidget(maxiCallsCheckBox, 2, 0);
  groupBoxLayout->addWidget(maxiCallsSpinbox_, 2, 1);
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