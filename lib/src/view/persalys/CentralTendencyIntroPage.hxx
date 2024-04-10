//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of central tendency analysis
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
#ifndef PERSALYS_INTROCENTRALTENDENCYPAGE_HXX
#define PERSALYS_INTROCENTRALTENDENCYPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QButtonGroup>

namespace PERSALYS
{

class PERSALYS_VIEW_API CentralTendencyIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, TaylorExpansionMoments};

  CentralTendencyIntroPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  OT::Description getInterestVariables() const;
  int getMethodId() const;

  virtual int nextId() const;
  virtual bool validatePage();

public slots:
  void updateFinalPage();

private:
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  QButtonGroup * methodGroup_;
  TemporaryLabel * errorMessageLabel_;
  OT::Description interestVariables_;
};
}
#endif
