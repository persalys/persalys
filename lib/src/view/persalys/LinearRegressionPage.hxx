//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a linear model analysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_LINEARREGRESSIONPAGE_HXX
#define PERSALYS_LINEARREGRESSIONPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/DoubleSpinBox.hxx"

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

namespace PERSALYS
{

class PERSALYS_VIEW_API LinearRegressionPage : public QWizardPage
{
  Q_OBJECT

public:
  LinearRegressionPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const DesignOfExperiment& doe) const;

protected:
  void buildInterface();

public slots:

private:
  QSpinBox * degreeSpinBox_ = 0;
  QCheckBox * interactionCheckBox_ = 0;
  OT::Description inputsNames_;
};
}
#endif
