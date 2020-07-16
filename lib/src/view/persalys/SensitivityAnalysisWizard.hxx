//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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

#include "persalys/AnalysisWizard.hxx"
#include "persalys/SensitivityIntroPage.hxx"
#include "persalys/SobolPage.hxx"
#include "persalys/SRCPage.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API SensitivityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestSensitivityAnalysisWizard;

public:
  enum {Page_Intro, Page_Sobol, Page_SRC};
  enum Method {Sobol, SRC};

  SensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

private:
  SensitivityIntroPage * introPage_;
  SobolPage * sobolPage_;
  SRCPage * srcPage_;
};
}
#endif
