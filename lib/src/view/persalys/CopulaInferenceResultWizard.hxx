//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose a copula inference result
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
#ifndef PERSALYS_COPULAINFERENCERESULTWIZARD_HXX
#define PERSALYS_COPULAINFERENCERESULTWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/Study.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/ResizableStackedWidget.hxx"

#include <QComboBox>
#include <QTableView>

namespace PERSALYS
{
class PERSALYS_API CopulaInferenceResultWizard : public Wizard
{
  Q_OBJECT

public:
  CopulaInferenceResultWizard(const Study &study, const OT::Description &variables, QWidget *parent = 0);

  OT::Distribution getCopula() const;

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void updateVariablesComboBox(int);
  void updateVariablesTable(const QString &text);

private:
  Study study_;
  OT::Description variables_;
  QComboBox * inferenceResultsComboBox_;
  QComboBox * variablesComboBox_;
  QTableView * tableView_;
  ResizableStackedWidget * inferenceResultStackWidget_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
