//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_METAMODELANALYSISWIZARD_HXX
#define PERSALYS_METAMODELANALYSISWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/MetaModelIntroPage.hxx"
#include "persalys/KrigingPage.hxx"
#include "persalys/FunctionalChaosPage.hxx"
#include "persalys/LinearRegressionPage.hxx"
#include "persalys/MetaModelValidationPage.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API MetaModelAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestMetaModelAnalysisWizard;

public:
  enum {Page_Intro, Page_ChaosMethod, Page_KrigingMethod, Page_Validation, Page_LinearRegressionMethod};

  MetaModelAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard = false, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

private:
  QList<DesignOfExperiment> doeList_;
  MetaModelIntroPage * introPage_ = 0;
  KrigingPage * krigingPage_ = 0;
  FunctionalChaosPage * functionalChaosPage_ = 0;
  MetaModelValidationPage * validationPage_ = 0;
  LinearRegressionPage * linearModelPage_ = 0;
};
}
#endif
