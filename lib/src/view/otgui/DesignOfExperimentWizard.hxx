//                                               -*- C++ -*-
/**
 *  @brief QWizard introducing the methods to define designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTWIZARD_HXX
#define OTGUI_DESIGNOFEXPERIMENTWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/DesignOfExperimentIntroPage.hxx"
#include "otgui/GridDesignPage.hxx"
#include "otgui/ProbabilisticDesignPage.hxx"
#include "otgui/ImportedDesignPage.hxx"

#include <QButtonGroup>

namespace OTGUI
{
class OTGUI_API DesignOfExperimentWizard : public AnalysisWizard
{
  Q_OBJECT

public:

  enum {Page_Intro, Page_Deterministic, Page_Probabilistic, Page_Import, Page_Evaluation};

  DesignOfExperimentWizard(const Analysis & designOfExperiment, QWidget *parent = 0);

  int nextId() const;
  virtual Analysis getAnalysis() const;

protected:
  void buildInterface();

private:
  DesignOfExperimentIntroPage * introPage_;
  GridDesignPage * gridPage_;
  ProbabilisticDesignPage * probaPage_;
  ImportedDesignPage * importPage_;
};
}
#endif
