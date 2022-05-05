//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a functional chaos analysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_FUNCTIONALCHAOSPAGE_HXX
#define PERSALYS_FUNCTIONALCHAOSPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/ValueLineEdit.hxx"

#include <QWizardPage>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>

namespace PERSALYS
{

class PERSALYS_VIEW_API FunctionalChaosPage : public QWizardPage
{
  Q_OBJECT

public:
  FunctionalChaosPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const DesignOfExperiment& doe) const;

  virtual bool validatePage();

protected:
  void buildInterface();

public slots:
  void updateInputSampleSize(DesignOfExperiment);
  void updateBasisLabel();
  void updateBasisSizeLabel();

private:
  OT::UnsignedInteger inputSampleSize_;
  OT::UnsignedInteger inputSampleDimension_;
  QLabel * basisLabel_;
  QLabel * basisSizeLabel_;
  QSpinBox * chaosDegreeSpinbox_;
  QCheckBox * sparseCheckBox_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
