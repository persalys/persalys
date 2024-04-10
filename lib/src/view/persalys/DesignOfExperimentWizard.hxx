//                                               -*- C++ -*-
/**
 *  @brief QWizard introducing the methods to define designs of experiments
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTWIZARD_HXX
#define PERSALYS_DESIGNOFEXPERIMENTWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/DesignOfExperimentIntroPage.hxx"
#include "persalys/GridDesignPage.hxx"
#include "persalys/ProbabilisticDesignPage.hxx"
#include "persalys/ImportedDesignPage.hxx"

#include <QButtonGroup>

namespace PERSALYS
{
class PERSALYS_VIEW_API DesignOfExperimentWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestDesignOfExperimentWizard;

public:

  enum {Page_Intro, Page_Deterministic, Page_Probabilistic, Page_Import};

  DesignOfExperimentWizard(const Analysis & designOfExperiment, QWidget *parent = nullptr);

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
