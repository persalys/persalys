//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the design of experiment and the method of metamodel analysis
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_INTROMETAMODELPAGE_HXX
#define OTGUI_INTROMETAMODELPAGE_HXX

#include "otgui/DesignOfExperiment.hxx"
#include "otgui/Analysis.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"

#include <QWizardPage>
#include <QLabel>
#include <QComboBox>
#include <QStandardItemModel>
#include <QButtonGroup>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

Q_DECLARE_METATYPE(OTGUI::DesignOfExperiment)

namespace OTGUI {

class OTGUI_API IntroMetaModelPage : public QWizardPage
{
  Q_OBJECT

public:
  IntroMetaModelPage(QWidget* parent=0);

  virtual int nextId() const;

  void initialize(const Analysis& analysis, QList<DesignOfExperiment> doesList);
  DesignOfExperiment getDesignOfExperiment() const;
  OT::Description getInterestVariables() const;

  virtual bool validatePage();

public slots:
  void updateDesignOfExperiment(int);
  void updateInterestVariables(QStringList);
signals:
  void methodChanged(int);
  void designOfExperimentChanged(DesignOfExperiment);

private:
  QComboBox * doesComboBox_;
  QStandardItemModel * doesComboBoxModel_;
  QLabel * doeLabel_;
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  QButtonGroup * methodGroup_;
  QLabel * errorMessageLabel_;
  OT::Description interestVariables_;
};
}
#endif