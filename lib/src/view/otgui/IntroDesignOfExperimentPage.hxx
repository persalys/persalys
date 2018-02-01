//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the type of design of experiments
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_INTRODESIGNOFEXPERIMENTPAGE_HXX
#define OTGUI_INTRODESIGNOFEXPERIMENTPAGE_HXX

#include "otgui/Analysis.hxx"

#include <QWizardPage>
#include <QButtonGroup>

namespace OTGUI
{

class OTGUI_API IntroDesignOfExperimentPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {deterministic, probabilistic, import};

  IntroDesignOfExperimentPage(QWidget* parent = 0);

  virtual int nextId() const;

  void initialize(const Analysis& analysis);

private:
  QButtonGroup * methodGroup_;
};
}
#endif
