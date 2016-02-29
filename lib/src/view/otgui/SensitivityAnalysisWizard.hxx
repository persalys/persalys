//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
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
#ifndef OTGUI_SENSITIVITYANALYSISWIZARD_HXX
#define OTGUI_SENSITIVITYANALYSISWIZARD_HXX

#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>
#include <QSpinBox>
#include <QGroupBox>
#include <QLabel>

namespace OTGUI {
class SensitivityAnalysisWizard : public OTguiWizard
{
  Q_OBJECT

public:
  enum Method {Sobol, SRC};

  SensitivityAnalysisWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  SensitivityAnalysisWizard(const Analysis & analysis);

  QString getAnalysisName() const;
  void validate();

protected:
  void buildInterface();

public slots:
  void updateMethodWidgets();
  void showHideAdvancedWidgets(bool);
  void sampleSizeChanged(int);
  void seedChanged(int);
signals:
  void analysisChanged(const Analysis & analysis);

private:
  Analysis analysis_;
  OTStudy * otStudy_;
  PhysicalModel physicalModel_;

  QButtonGroup * methodGroup_;
  QGroupBox * advancedGroup_;
  QWidget * advancedWidgets_;
  QSpinBox * sampleSizeSpinbox_;
  QWidget * sobolWidgets_;
  QLabel * totalNbSimuLabel_;
  QSpinBox * seedSpinbox_;
};
}
#endif