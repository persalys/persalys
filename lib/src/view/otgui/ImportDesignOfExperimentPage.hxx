//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
#ifndef OTGUI_IMPORTDESIGNOFEXPERIMENTPAGE_HXX
#define OTGUI_IMPORTDESIGNOFEXPERIMENTPAGE_HXX

#include "otgui/ImportDataPage.hxx"
#include "otgui/DesignOfExperiment.hxx"
#include "otgui/FromFileDesignOfExperiment.hxx"

namespace OTGUI {
class ImportDesignOfExperimentPage : public ImportDataPage
{
  Q_OBJECT

public:
  ImportDesignOfExperimentPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  virtual void setTable(OT::NumericalSample & sample);
  virtual void setFileName(const QString & fileName);

public slots:
  void columnNameChanged();
signals:
  void designOfExperimentChanged(const DesignOfExperiment & designOfExperiment);

private:
  FromFileDesignOfExperiment designOfExperiment_;
};
}
#endif