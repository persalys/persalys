//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_IMPORTDESIGNOFEXPERIMENTPAGE_HXX
#define PERSALYS_IMPORTDESIGNOFEXPERIMENTPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/ImportedDesignOfExperiment.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QLineEdit>

namespace PERSALYS
{
class PERSALYS_API ImportedDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  ImportedDesignPage(QWidget *parent = 0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis();
  bool validatePage();

protected:
  void buildInterface();
  virtual void setTable(const QString& fileName);
  void setData(const QString & fileName);

public slots:
  void openFileRequested();
  void columnNameChanged();

private:
  ImportedDesignOfExperiment designOfExperiment_;
  bool pageValidity_;
  QLineEdit * filePathLineEdit_;
  ExportableTableView * dataPreviewTableView_;
  QLabel * DOESizeLabel_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
