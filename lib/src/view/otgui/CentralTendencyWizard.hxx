//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_CENTRALTENDENCYWIZARD_HXX
#define OTGUI_CENTRALTENDENCYWIZARD_HXX

#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>
#include <QSpinBox>
#include <QCheckBox>

namespace OTGUI {
class CentralTendencyWizard : public OTguiWizard
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, TaylorExpansionsMoments};

  CentralTendencyWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  CentralTendencyWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets();
  void showHideAdvancedWidgets(bool);
  void confidenceIntervalRequired(bool);
  void nbSimuChanged(int);
  void levelConfidenceIntervalChanged(double);
  void seedChanged(int);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QWidget * advancedWidgets_;
  QWidget * monteCarloWidget_;
  QWidget * TaylorExpansionsWidget_;
  QSpinBox * nbSimuSpinbox_;
  QSpinBox * seedSpinbox_;
  QCheckBox * confidenceIntervalCheckBox_;
  QDoubleSpinBox * levelConfidenceIntervalSpinbox_;
};
}
#endif