//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define deterministic designs of experiments
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_DETERMINISTICDESIGNPAGE_HXX
#define OTGUI_DETERMINISTICDESIGNPAGE_HXX

#include "otgui/DesignOfExperiment.hxx"
#include "otgui/ExperimentTableModel.hxx"

#include <QWizardPage>

namespace OTGUI {
class DeterministicDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  DeterministicDesignPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  void buildInterface();

public slots:
signals:
  void designOfExperimentChanged(const DesignOfExperiment & designOfExperiment);

private:
  ExperimentTableModel * model_;
};
}
#endif