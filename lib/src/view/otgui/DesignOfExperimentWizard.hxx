//                                               -*- C++ -*-
/**
 *  @brief QWizard introducing the methods to define designs of experiments
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_DESIGNOFEXPERIMENTWIZARD_HXX
#define OTGUI_DESIGNOFEXPERIMENTWIZARD_HXX

#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>

namespace OTGUI {

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
  enum Method {deterministic, probabilistic, import};

  IntroPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  int nextId() const;

private:
  QButtonGroup * methodGroup_;
};


class DesignOfExperimentWizard : public OTguiWizard
{
  Q_OBJECT

public:

  enum {Page_Intro, Page_Deterministic, Page_Probabilistic, Page_Import};

  DesignOfExperimentWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  DesignOfExperimentWizard(const DesignOfExperiment & designOfExperiment);

  int nextId() const;
  void validate();

protected:
  void buildInterface();

public slots:
  void setDesignOfExperiment(DesignOfExperiment & designOfExperiment);

private:
  IntroPage * introPage_;
  DesignOfExperiment designOfExperiment_;
  OTStudy * otStudy_;
};
}
#endif