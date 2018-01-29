//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose a copula inference result
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
#ifndef OTGUI_COPULAINFERENCERESULTWIZARD_HXX
#define OTGUI_COPULAINFERENCERESULTWIZARD_HXX

#include "otgui/OTguiWizard.hxx"
#include "otgui/OTStudy.hxx"
#include "otgui/TemporaryLabel.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <QComboBox>
#include <QTableView>

namespace OTGUI
{
class OTGUI_API CopulaInferenceResultWizard : public OTguiWizard
{
  Q_OBJECT

public:
  CopulaInferenceResultWizard(const OTStudy &otStudy, const OT::Description &variables, QWidget *parent = 0);

  OT::Copula getCopula() const;

  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void updateVariablesComboBox(int);
  void updateVariablesTable(const QString &text);

private:
  OTStudy otStudy_;
  OT::Description variables_;
  QComboBox * inferenceResultsComboBox_;
  QComboBox * variablesComboBox_;
  QTableView * tableView_;
  ResizableStackedWidget * inferenceResultStackWidget_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
