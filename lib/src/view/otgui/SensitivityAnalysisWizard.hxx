//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
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
#ifndef OTGUI_SENSITIVITYANALYSISWIZARD_HXX
#define OTGUI_SENSITIVITYANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/IntroSensitivityPage.hxx"
#include "otgui/SobolPage.hxx"
#include "otgui/SRCPage.hxx"

namespace OTGUI {
class OTGUI_API SensitivityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum {Page_Intro, Page_Sobol, Page_SRC};
  enum Method {Sobol, SRC};

  SensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent=0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

private:
  IntroSensitivityPage * introPage_;
  SobolPage * sobolPage_;
  SRCPage * srcPage_;
};
}
#endif