//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose an inference result
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_INFERENCERESULTWIZARD_HXX
#define PERSALYS_INFERENCERESULTWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/Study.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/InferenceResultWidget.hxx"

#include <QComboBox>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_VIEW_API InferenceResultWizard : public Wizard
{
  Q_OBJECT

public:
  InferenceResultWizard(const Study& study, QWidget* parent = 0);

  OT::Distribution getDistribution() const;

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void clearErrorMessage();
  void updateVariablesComboBox(int);
  void updateInferenceResultWidget(int);

private:
  Study study_;
  QComboBox * inferenceResultsComboBox_;
  QComboBox * variablesComboBox_;
  InferenceResultWidget * inferenceResultWidget_;
  QLabel * errorMessageLabel_;
};
}
#endif
