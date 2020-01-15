//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the design of experiments and the method of metamodel analysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_INTROMETAMODELPAGE_HXX
#define PERSALYS_INTROMETAMODELPAGE_HXX

#include "persalys/DesignOfExperiment.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QComboBox>
#include <QStandardItemModel>
#include <QButtonGroup>

namespace PERSALYS
{

class PERSALYS_API MetaModelIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {Chaos, Kriging};

  MetaModelIntroPage(QWidget* parent = 0);

  virtual int nextId() const;

  void initialize(const Analysis& analysis, QList<DesignOfExperiment> doesList);
  DesignOfExperiment getDesignOfExperiment() const;
  OT::Description getInterestVariables() const;
  int getMethodId() const;

  virtual bool validatePage();

public slots:
  void updateDesignOfExperiment(int);
  void updateInterestVariables(QStringList);
signals:
  void designOfExperimentChanged(DesignOfExperiment);

private:
  QComboBox * doesComboBox_;
  QStandardItemModel * doesComboBoxModel_;
  QLabel * doeLabel_;
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  QButtonGroup * methodGroup_;
  TemporaryLabel * errorMessageLabel_;
  OT::Description interestVariables_;
};
}
#endif
