//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_MODELEVALUATIONWIZARD_HXX
#define OTGUI_MODELEVALUATIONWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"
#include "otgui/TemporaryLabel.hxx"

#include <QTableWidget>
#include <QLabel>

namespace OTGUI
{
class OTGUI_API ModelEvaluationWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  ModelEvaluationWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

private:
  QTableWidget * table_;
  OutputsSelectionGroupBox * outputsGroupBox_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
