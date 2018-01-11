//                                               -*- C++ -*-
/**
 *  @brief QWidget to define copula
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
#ifndef OTGUI_COPULAWIDGET_HXX
#define OTGUI_COPULAWIDGET_HXX

#include "otgui/PhysicalModel.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/ValueLineEdit.hxx"
#include "otgui/TemporaryLabel.hxx"

namespace OTGUI
{
class OTGUI_API CopulaWidget : public QWidget
{
  Q_OBJECT

public:
  CopulaWidget(const PhysicalModel &model, const OT::Copula &copula, QWidget *parent = 0);

public slots:
  void updatePlots();
  void updateCopulaFromLineEdit();
  void updateCopulaFromCorrTable(const OT::Copula &copula);
  void openDocUrl();

private:
  PhysicalModel physicalModel_;
  OT::Copula copula_;
  QVector<PlotWidget*> listPlot_;
  ValueLineEdit * paramValueEdit_;
  TemporaryLabel * errorMessage_;
};
}
#endif
