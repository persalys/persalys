//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list covariance models for HSCI sensitivity analysis
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

#ifndef PERSALYS_HSICCOVARIANCEMODELSTABLEMODEL_HXX
#define PERSALYS_HSICCOVARIANCEMODELSTABLEMODEL_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QAbstractTableModel>
#include <QSet>

#include <openturns/Collection.hxx>
#include <openturns/CovarianceModel.hxx>

namespace PERSALYS
{
class PERSALYS_MODEL_API HSICCovarianceModelsTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  enum class CovarianceModelType
  {
    Matern = 0,
    SquaredExponential = 1,
    AbsoluteExponential = 2,
    GeneralizedExponential = 3
  };

  enum class NuType
  {
    OneHalf = 0,
    ThreeHalf = 1,
    FiveHalf = 2
  };

  explicit HSICCovarianceModelsTableModel(const OT::Description & variableNames, QObject *parent = nullptr);
  
  void setVariablesNames(const OT::Description & variableNames);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;

  OT::Collection<OT::CovarianceModel> getCovarianceModels() const;
  void setCovarianceModels(const OT::Collection<OT::CovarianceModel> & models);

  void applyToAll(int row);
  bool hasErrors() const;

signals:
  void errorMessageChanged(QString);

private:
  OT::Description variableNames_;
  QList<CovarianceModelType> modelIndices_;
  QList<NuType> nuIndices_;
  QList<double> pValues_;
  QSet<int> pErrorRows_;
};

} // namespace PERSALYS

#endif // PERSALYS_HSICCOVARIANCEMODELSTABLEMODEL_HXX