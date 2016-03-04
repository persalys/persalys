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
#ifndef OTGUI_IMPORTTABLEPAGE_HXX
#define OTGUI_IMPORTTABLEPAGE_HXX

#include "OTguiTableView.hxx"
#include "otgui/DesignOfExperiment.hxx"

#include <QWizardPage>
#include <QLineEdit>

namespace OTGUI {
class ImportTablePage : public QWizardPage
{
  Q_OBJECT

public:
  ImportTablePage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  void buildInterface();
  void loadFile();

public slots:
  void openFileRequested();
  void columnChanged(Qt::Orientation, int, int);
signals:
  void dataImported(OT::NumericalSample);

private:
  DesignOfExperiment designOfExperiment_;
  bool pageValidity_;
  QLineEdit * filePathLineEdit_;
  OTguiTableView * dataPreviewTableView_;
};
}
#endif