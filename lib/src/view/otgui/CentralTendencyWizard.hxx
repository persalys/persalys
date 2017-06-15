//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
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
#ifndef OTGUI_CENTRALTENDENCYWIZARD_HXX
#define OTGUI_CENTRALTENDENCYWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/IntroCentralTendencyPage.hxx"
#include "otgui/MonteCarloPage.hxx"

namespace OTGUI {
class OTGUI_API CentralTendencyWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum {Page_Intro, Page_MonteCarlo};

  CentralTendencyWizard(const Analysis& analysis, QWidget* parent=0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

private:
  IntroCentralTendencyPage * introPage_;
  MonteCarloPage * monteCarloPage_;
};
}
#endif