//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a wizard for parameters definition
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
#ifndef OTGUI_PARAMETERSDEFINITIONWIZARD_HXX
#define OTGUI_PARAMETERSDEFINITIONWIZARD_HXX

#include <openturns/OTType.hxx>

#include <QWizard>
#include <QStandardItemModel>

namespace OTGUI {
class ParametersTableModel: public QStandardItemModel
{
public:
  ParametersTableModel(const OT::Description& names, const OT::NumericalPoint& values, const QStringList& labels, QObject* parent = 0);

  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant data(const QModelIndex& index, int role) const;
  bool setData(const QModelIndex& index, const QVariant& value, int role);
  OT::NumericalPoint getValues() const;

private:
    OT::Description names_;
    OT::NumericalPoint values_;
};

class ParametersDefinitionWizard: public QWizard
{
  Q_OBJECT

public:
  ParametersDefinitionWizard(const OT::Description& names, const OT::NumericalPoint& values, const QStringList& labels, QWidget* parent=0);

  OT::NumericalPoint getValues() const;

private:
  ParametersTableModel * tableModel_;
};
}
#endif