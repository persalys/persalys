//                                               -*- C++ -*-
/**
 *  @brief QWizardPage for meta model validation
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
#ifndef PERSALYS_METAMODELVALIDATIONPAGE_HXX
#define PERSALYS_METAMODELVALIDATIONPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/UIntSpinBox.hxx"

#include <QWizardPage>
#include <QCheckBox>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_VIEW_API MetaModelValidationPage : public QWizardPage
{
  Q_OBJECT

public:
  MetaModelValidationPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  void updateMetamodelValidation(Analysis& analysis);

  virtual bool validatePage();

protected:
  void buildInterface();

public slots:
  void updateInputSampleSize(DesignOfExperiment);
  void percentageValueChanged(double);

private:
  OT::UnsignedInteger inputSampleSize_;
  QCheckBox * analyticalCheckBox_;
  QCheckBox * kFoldCheckBox_;
  QCheckBox * testSampleCheckBox_;
  QLabel * trainingSampleSize_;
//   QCheckBox * looCheckBox_;
  UIntSpinBox * percentageOfPointsSpinBox_;
  UIntSpinBox * seedTestSampleSpinBox_;
  UIntSpinBox * nbFoldsSpinBox_;
  UIntSpinBox * seedKFoldSpinBox_;
  QLabel * errorMessageLabel_;
};
}
#endif
