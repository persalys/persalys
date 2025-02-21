//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of screening analysis
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
#ifndef PERSALYS_INTROSCREENINGPAGE_HXX
#define PERSALYS_INTROSCREENINGPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QButtonGroup>

namespace PERSALYS
{

class PERSALYS_VIEW_API ScreeningIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {Morris};

  ScreeningIntroPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  OT::Description getInterestVariables() const;

  virtual int nextId() const;
  virtual bool validatePage();

private:
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
//   QButtonGroup * methodGroup_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
