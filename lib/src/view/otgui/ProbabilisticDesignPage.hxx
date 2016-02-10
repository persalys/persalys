//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiment
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
#ifndef OTGUI_PROBABILISTICDESIGNPAGE_HXX
#define OTGUI_PROBABILISTICDESIGNPAGE_HXX

#include "otgui/DesignOfExperiment.hxx"

#include <QWizardPage>

namespace OTGUI {
class ProbabilisticDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  ProbabilisticDesignPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  void buildInterface();

public slots:
  void setDesignOfExperiment(DesignOfExperiment &);
signals:
  void designOfExperimentChanged(const DesignOfExperiment & designOfExperiment);

private:
  DesignOfExperiment designOfExperiment_;
  bool pageValidity_;
  QWidget * designsWidget_ ;
};
}
#endif