//                                               -*- C++ -*-
/**
 *  @brief QDialog to define values
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
#ifndef PERSALYS_EDITVALUESWIZARD_HXX
#define PERSALYS_EDITVALUESWIZARD_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Point.hxx>

#include <QWizard>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QStandardItemModel>

namespace PERSALYS
{

class EditValuesWizard : public QWizard
{
  Q_OBJECT

public:
  EditValuesWizard(const QString &variableName, const OT::Point &values, QWidget *parent = 0);

  OT::Point getValues() const;
  virtual bool validateCurrentPage();

private slots:
  void removeSelectedValues();
  void checkButtons();
  void addValue();

private:
  void addAValue(double v);
  void insertAValue(double value);
  void check();

  QStandardItemModel * model_;
  QSortFilterProxyModel * proxy_;
  QTableView * valueTable_;
  QLabel * valueNumber_;
  QDoubleSpinBox * addedValueBox_;
  QPushButton * removeButton_;
  QDoubleSpinBox * deltaBox_;
  QSpinBox * nbPointsBox_;

  OT::Point values_;
};
}
#endif
