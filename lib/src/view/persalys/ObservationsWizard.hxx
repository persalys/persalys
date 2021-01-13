//                                               -*- C++ -*-
/**
 *  @brief QWizard to import sample to define Observations
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_OBSERVATIONSWIZARD_HXX
#define PERSALYS_OBSERVATIONSWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/Observations.hxx"
#include "persalys/ImportSampleWidget.hxx"

#include <QWizardPage>

namespace PERSALYS
{
class PERSALYS_VIEW_API ImportObservationsPage : public QWizardPage
{
  Q_OBJECT

  friend class TestObservationsWizard;

public:
  ImportObservationsPage(QWidget *parent = 0);

  void initialize(const DesignOfExperiment& designOfExp);
  DesignOfExperiment getDesignOfExperiment();
  bool validatePage();

protected:
  void buildInterface();

public slots:
  void setTable(const QString& fileName);
  void checkColumns();

private:
  ImportSampleWidget * sampleWidget_;
  Observations observations_;
};


class PERSALYS_VIEW_API ObservationsWizard : public Wizard
{
  Q_OBJECT

  friend class TestObservationsWizard;

public:
  ObservationsWizard(const DesignOfExperiment &designOfExp, QWidget *parent = 0)
  : Wizard(parent)
  {
    docLink_ = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#observationwizard";

    page_ = new ImportObservationsPage(this);
    page_->initialize(designOfExp);
    setPage(0, page_);
  }
  DesignOfExperiment getDesignOfExperiment()
  {
    return page_->getDesignOfExperiment();
  }
private:
  ImportObservationsPage * page_;
};
}
#endif
