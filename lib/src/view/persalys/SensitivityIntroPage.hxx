//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_INTROSENSITIVITYPAGE_HXX
#define PERSALYS_INTROSENSITIVITYPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QButtonGroup>

namespace PERSALYS
{

class PERSALYS_VIEW_API SensitivityIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {Sobol, SRC};

  SensitivityIntroPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  OT::Description getInterestVariables() const;
  int getMethodId() const;

  virtual int nextId() const;
  virtual bool validatePage();

private:
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  QButtonGroup * methodGroup_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
