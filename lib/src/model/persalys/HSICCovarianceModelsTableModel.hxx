//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list covariance models for HSCI sensitivity analysis
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

#ifndef PERSALYS_HSICCOVARIANCEMODELSTABLEMODEL_HXX
#define PERSALYS_HSICCOVARIANCEMODELSTABLEMODEL_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QAbstractTableModel>

#include <openturns/Collection.hxx>
#include <openturns/CovarianceModel.hxx>

namespace PERSALYS
{
class PERSALYS_MODEL_API HSICCovarianceModelsTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit HSICCovarianceModelsTableModel(const OT::Description & variableNames, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;

  OT::Collection<OT::CovarianceModel> getCovarianceModels() const;
  void setCovarianceModels(const OT::Collection<OT::CovarianceModel> & models);

private:
  OT::Description variableNames_;
  QList<int> modelIndices_;   // 0=Matern, 1=SquaredExponential, 2=AbsoluteExponential, 3=GeneralizedExponential
  QList<int> nuIndices_;      // 0=1/2, 1=3/2, 2=5/2
};

} // namespace PERSALYS

#endif // PERSALYS_HSICCOVARIANCEMODELSTABLEMODEL_HXX