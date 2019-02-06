//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a ScreeningAnalysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_SCREENINGANALYSISWIZARD_HXX
#define OTGUI_SCREENINGANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/ScreeningIntroPage.hxx"
#include "otgui/MorrisPage.hxx"

namespace OTGUI
{
class OTGUI_API ScreeningAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestScreeningAnalysisWizard;

public:
  enum {Page_Intro, Page_Morris, Page_Morris2};
  enum Method {Morris};

  ScreeningAnalysisWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

private:
  ScreeningIntroPage * introPage_;
  MorrisPage * morrisPage_;
  MorrisSecondPage * morrisSecondPage_;
};
}
#endif
