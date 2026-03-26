//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_SENSITIVITYANALYSISWIZARD_HXX
#define PERSALYS_SENSITIVITYANALYSISWIZARD_HXX

#include "persalys/Analysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/AnalysisWizard.hxx"
#include "persalys/SobolPage.hxx"

#include <QWizardPage>
#include <QButtonGroup>

namespace PERSALYS
{

class PERSALYS_VIEW_API SensitivityIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SensitivityIntroPage(QWidget* parent = nullptr);

  void initialize(const Analysis& analysis);
  OT::Description getInterestVariables() const;

  int nextId() const override;
  bool validatePage() override;

private:
  OutputsSelectionGroupBox * outputsSelectionGroupBox_ = nullptr;
  QButtonGroup * methodGroup_ = nullptr;
  ErrorWidget * errorWidget_ = nullptr;
};

class PERSALYS_VIEW_API SensitivityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestSensitivityAnalysisWizard;

public:
  enum {Page_Intro, Page_Sobol};

  explicit SensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent = nullptr);

  Analysis getAnalysis() const override;
  int nextId() const override;

protected:
  void buildInterface();

private:
  SensitivityIntroPage * introPage_;
  SobolPage * sobolPage_;
};
}
#endif
