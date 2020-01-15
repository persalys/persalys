//                                               -*- C++ -*-
/**
 *  @brief QWidget to define copula
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
#ifndef PERSALYS_COPULAWIDGET_HXX
#define PERSALYS_COPULAWIDGET_HXX

#include "persalys/PhysicalModel.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/ValueLineEdit.hxx"

#include <QGroupBox>
#include <QVBoxLayout>

namespace PERSALYS
{
class PERSALYS_API CopulaWidget : public QWidget
{
  Q_OBJECT

public:
  CopulaWidget(const PhysicalModel &model, const OT::Distribution &copula, QWidget *parent = 0);
  void setCopula(const OT::Distribution &copula);

public slots:
  void updateCopulaFromLineEdit();
  void updateCopulaFromCorrTable(const OT::Distribution &copula);
  void openDocUrl();
signals:
  void emitErrorMessage(const QString &message);

protected:
  void updatePlots();
  void updateParameters();

private:
  PhysicalModel physicalModel_;
  OT::Distribution copula_;
  QVBoxLayout * parameterLayout_;
  QGroupBox * paramEditor_;
  ValueLineEdit * paramValueEdit_;
  QVector<PlotWidget*> listPlot_;
};
}
#endif
