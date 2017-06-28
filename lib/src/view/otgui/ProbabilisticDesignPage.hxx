//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiment
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
#ifndef OTGUI_PROBABILISTICDESIGNPAGE_HXX
#define OTGUI_PROBABILISTICDESIGNPAGE_HXX

#include "otgui/DesignOfExperiment.hxx"
#include "otgui/LogSpinBox.hxx"

#include <QWizardPage>
#include <QButtonGroup>

namespace OTGUI {
class OTGUI_API ProbabilisticDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  enum design {LHS, MonteCarlo, QuasiMonteCarlo};

  ProbabilisticDesignPage(QWidget* parent = 0);

  void initialize(const DesignOfExperiment& designOfExperiment);
  DesignOfExperiment getDesignOfExperiment(const OT::String& name, const PhysicalModel& model) const;

protected:
  void buildInterface();

public slots:
signals:
  void designOfExperimentChanged(const DesignOfExperiment & designOfExperiment);

private:
  QButtonGroup * designsGroup_;
  LogSpinBox * sampleSizeSpinbox_;
  QSpinBox * seedSpinbox_;
};
}
#endif