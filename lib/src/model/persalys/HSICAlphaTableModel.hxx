//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to configure alpha parameter for HSIC filter/weight functions
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#ifndef PERSALYS_HSICALPHATABLEMODEL_HXX
#define PERSALYS_HSICALPHATABLEMODEL_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QAbstractTableModel>
#include <QSet>

#include <openturns/Point.hxx>
#include <openturns/Description.hxx>

namespace PERSALYS
{
class PERSALYS_MODEL_API HSICAlphaTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit HSICAlphaTableModel(const OT::Description & variableNames,
                               QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;

  void setAlphas(const OT::Point & alphas);
  OT::Point getAlphas() const;
  bool hasErrors() const;

signals:
  void errorMessageChanged(QString);

private:
  OT::Description variableNames_;
  QList<double> alphaValues_;
  QSet<int> errorRows_;
};

} // namespace PERSALYS

#endif // PERSALYS_HSICALPHATABLEMODEL_HXX
