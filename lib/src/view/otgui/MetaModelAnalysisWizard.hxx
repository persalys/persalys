//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
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
#ifndef OTGUI_METAMODELANALYSISWIZARD_HXX
#define OTGUI_METAMODELANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/OTguiItem.hxx"
#include "otgui/IntroMetaModelPage.hxx"
#include "otgui/KrigingPage.hxx"
#include "otgui/FunctionalChaosPage.hxx"

namespace OTGUI {
class OTGUI_API MetaModelAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum Method {chaos, kriging};
  enum {Page_Intro, Page_KrigingMethod, Page_ChaosMethod};

  MetaModelAnalysisWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard=false, QWidget* parent=0);

  virtual int nextId() const;
  virtual bool validateCurrentPage();

protected:
  void buildInterface();

private:
  QList<DesignOfExperiment> doeList_;
  IntroMetaModelPage * introPage_;
  KrigingPage * krigingPage_;
  FunctionalChaosPage * functionalChaosPage_;
};
}
#endif