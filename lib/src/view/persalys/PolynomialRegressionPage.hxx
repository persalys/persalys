//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a linear model analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_POLYNOMIALREGRESSIONPAGE_HXX
#define PERSALYS_POLYNOMIALREGRESSIONPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/DoubleSpinBox.hxx"

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>

namespace PERSALYS
{

class PERSALYS_VIEW_API PolynomialRegressionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit PolynomialRegressionPage(QWidget* parent = nullptr);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const DesignOfExperiment& doe) const;

protected:
  void buildInterface();

private:
  QSpinBox        * degreeSpinBox_            = nullptr;
  QCheckBox       * interactionCheckBox_      = nullptr;
  QCheckBox       * stepwiseCheckBox_         = nullptr;
  QComboBox       * directionComboBox_        = nullptr;
  QComboBox       * penaltyCriteriaComboBox_  = nullptr;

  static QString FORWARD, BACKWARD, BOTH, BIC, AIC;
};
}
#endif
