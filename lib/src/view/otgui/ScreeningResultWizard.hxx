//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose a screening result
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_SCREENINGRESULTWIZARD_HXX
#define OTGUI_SCREENINGRESULTWIZARD_HXX

#include "otgui/Wizard.hxx"
#include "otgui/Study.hxx"
#include "otgui/MorrisResultTableModel.hxx"

#include <QComboBox>
#include <QLabel>

namespace OTGUI
{
class OTGUI_API ScreeningResultWizard : public Wizard
{
  Q_OBJECT

public:
  ScreeningResultWizard(const Study& otStudy, const PhysicalModel& model, QWidget* parent = 0);

  OT::Indices getInputsSelection() const;

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void clearErrorMessage();
  void updateVariablesComboBox(const int);
  void updateTableModel(const QString&);

private:
  Study otStudy_;
  PhysicalModel model_;
  QComboBox * screeningResultsComboBox_;
  QComboBox * variablesComboBox_;
  MorrisResultTableModel * tableModel_;
  QLabel * errorMessageLabel_;
};
}
#endif
