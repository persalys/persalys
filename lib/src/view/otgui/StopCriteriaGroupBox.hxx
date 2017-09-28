//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for stop criteria
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
#ifndef OTGUI_STOPCRITERIAGROUPBOX_HXX
#define OTGUI_STOPCRITERIAGROUPBOX_HXX

#include "otgui/TimeLineEdit.hxx"
#include "otgui/LogSpinBox.hxx"

#include "openturns/OTType.hxx"

#include <QGroupBox>
#include <QCheckBox>

namespace OTGUI
{
class OTGUI_API StopCriteriaGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  // constructor
  StopCriteriaGroupBox(QWidget* parent = 0);

  OT::UnsignedInteger getMaximumCalls() const;
  void setMaximumCalls(const OT::UnsignedInteger maxi);

  double getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const double coef);

  OT::UnsignedInteger getMaximumElapsedTime() const;
  void setMaximumElapsedTime(const OT::UnsignedInteger seconds);

  bool isMaxElapsedTimeValid() const;
  bool isMaxCallsRequired() const;
  bool isValid() const;

protected:
  void buildInterface();

public slots:
  void maxiCoefficientOfVariationRequired(bool);
  void maxiTimeRequired(bool);
  void maxiTimeChanged();
  void maxiCallsRequired(bool);

signals:
  void maxiCoefficientOfVariationChanged(double);
  void maxiTimeChanged(int);
  void maxiCallsChanged(double);
  void clearErrorMessageLabel();

private:
  QCheckBox * maxiCoefOfVarCheckBox_;
  DoubleSpinBox * maxiCoefficientOfVariationSpinbox_;
  QCheckBox * maxiTimeCheckBox_;
  TimeLineEdit * maxTimeLineEdit_;
  QCheckBox * maxiCallsCheckBox_;
  LogSpinBox * maxiCallsSpinbox_;
};
}
#endif