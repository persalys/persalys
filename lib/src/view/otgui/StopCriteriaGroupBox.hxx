//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for stop criteria
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
#ifndef OTGUI_STOPCRITERIAGROUPBOX_HXX
#define OTGUI_STOPCRITERIAGROUPBOX_HXX

#include "otgui/TimeLineEdit.hxx"
#include "otgui/LogSpinBox.hxx"

#include <openturns/OTType.hxx>

#include <QGroupBox>
#include <QCheckBox>

namespace OTGUI
{
class OTGUI_API StopCriteriaGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  enum Criteria {Time_Calls, Time_Calls_CoefVar, Time_Calls_CILength};

  // constructor
  StopCriteriaGroupBox(const Criteria criteria = Time_Calls_CoefVar, QWidget* parent = 0);

  OT::UnsignedInteger getMaximumCalls() const;
  void setMaximumCalls(const OT::UnsignedInteger maxi);

  OT::Scalar getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const OT::Scalar coef);

  OT::Scalar getMaximumConfidenceIntervalLength() const;
  void setMaximumConfidenceIntervalLength(const OT::Scalar length);

  OT::UnsignedInteger getMaximumElapsedTime() const;
  void setMaximumElapsedTime(const OT::UnsignedInteger seconds);

  bool isMaxElapsedTimeValid() const;
  bool isMaxCallsRequired() const;
  bool isValid() const;

protected:
  void buildInterface();

signals:
  void criteriaChanged();

private:
  Criteria criteria_;
  QCheckBox * maxiCoefOfVarCheckBox_;
  DoubleSpinBox * maxiCoefficientOfVariationSpinbox_;
  QCheckBox * maxiCILengthCheckBox_;
  DoubleSpinBox * maxiCILengthSpinbox_;
  QCheckBox * maxiTimeCheckBox_;
  TimeLineEdit * maxTimeLineEdit_;
  QCheckBox * maxiCallsCheckBox_;
  LogSpinBox * maxiCallsSpinbox_;
};
}
#endif
