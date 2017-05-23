//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define simulation reliability analysis
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
#ifndef OTGUI_SIMULATIONRELIABILITYPAGE_HXX
#define OTGUI_SIMULATIONRELIABILITYPAGE_HXX

#include "otgui/Analysis.hxx"
#include "otgui/LimitState.hxx"
#include "otgui/StopCriteriaGroupBox.hxx"
#include "otgui/BlockSizeGroupBox.hxx"

#include <QWizardPage>
#include <QLabel>

namespace OTGUI {
class OTGUI_API SimulationReliabilityPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {MonteCarlo = 1, FORM_IS = 2};

  SimulationReliabilityPage(QWidget* parent=0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const LimitState& limitState) const;

  virtual int nextId() const;
  virtual bool validatePage();

protected:
  void buildInterface();

public slots:
  void updateMethod(int);
signals:
  void methodChanged(int);

private:
  Method method_;
  StopCriteriaGroupBox * stopCriteriaGroupBox_;
  BlockSizeGroupBox * blockSizeGroupBox_;
  QSpinBox * seedSpinbox_;
  QLabel * errorMessageLabel_;
};
}
#endif