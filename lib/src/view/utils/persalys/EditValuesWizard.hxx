//                                               -*- C++ -*-
/**
 *  @brief QDialog to define values
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
#ifndef PERSALYS_EDITVALUESWIZARD_HXX
#define PERSALYS_EDITVALUESWIZARD_HXX

#include "persalys/SampleTableModel.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/ImportSampleWidget.hxx"


#include <openturns/Distribution.hxx>

#include <QWizard>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QPushButton>

namespace PERSALYS
{

class PERSALYS_UTILS_API EditValuesWizard : public QWizard
{
  Q_OBJECT

public:
  explicit EditValuesWizard(QWidget *parent = nullptr, OT::UnsignedInteger nMinValues = 2, OT::Scalar factor = 1.);
  explicit EditValuesWizard(const OT::Sample &values, QWidget *parent = nullptr, OT::UnsignedInteger nMinValues = 2, OT::Scalar factor = 1.);
  EditValuesWizard(const QString &variableName, const OT::Point &values, QWidget *parent = nullptr, OT::UnsignedInteger nMinValues = 2, OT::Scalar factor = 1.);

  OT::Point getValues(const OT::UnsignedInteger index = 0) const;
  bool validateCurrentPage() override;

  virtual OT::Distribution getDistribution() const;

private slots:
  void removeSelectedValues();
  void checkButtons();
  virtual void addValue(OT::Scalar factor);
  virtual void importSample();

protected:
  void buildInterface();
  void check();

  SampleTableModel * model_ = nullptr;
  QSortFilterProxyModel * proxy_ = nullptr;
  ErrorWidget * errorWidget_ = nullptr;
  bool sortValues_ = true;

private:
  OT::Description     sampleDescription_;
  QTableView          * valueTable_       = nullptr;
  QLabel              * valueNumber_      = nullptr;
  QPushButton         * removeButton_     = nullptr;
  OT::UnsignedInteger nMinValues_         = 2;
  OT::Scalar          factor_             = 1.;

};


class PERSALYS_UTILS_API UserDefinedWizard : public EditValuesWizard
{
  Q_OBJECT

public:
  explicit UserDefinedWizard(const OT::Distribution::PointWithDescriptionCollection &parameters, QWidget *parent = nullptr);

  OT::Distribution getDistribution() const override;

private slots:
  void addValue(OT::Scalar) override;
};

class PERSALYS_UTILS_API HistogramWizard: public EditValuesWizard
{
  Q_OBJECT

public:
  HistogramWizard(const OT::Scalar first, const OT::Point &widths, const OT::Point &heights, QWidget *parent = nullptr);

  OT::Distribution getDistribution() const override;

private slots:
  void addValue(OT::Scalar) override;

private:
  OT::Scalar first_ = 0.;
  OT::Description sampleDescription_;
};

class PERSALYS_UTILS_API WeightTableModel : public SampleTableModel
{
  Q_OBJECT

public:
  WeightTableModel(const OT::Sample & data, QObject * parent)
    : SampleTableModel(data, true, false, OT::Description(), parent)
  {
    Q_ASSERT(data.getDimension() == 2);
  }

  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override
  {
    if (!index.isValid())
      return false;
    if (role == Qt::EditRole && index.column() == 1 && !(value.toDouble() > 0))
    {
      emit errorMessageChanged(tr("Weight must be in strictly positive"));
      return false;
    }

    return SampleTableModel::setData(index, value, role);
  }
};

class PERSALYS_UTILS_API HistogramTableModel: public SampleTableModel
{
  Q_OBJECT

public:
  HistogramTableModel(const OT::Sample &data, QObject *parent):
    SampleTableModel(data, true, false, OT::Description(), parent)
  {
    Q_ASSERT(data.getDimension() == 2);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
  {
    if (!index.isValid())
      return false;
    if (role == Qt::EditRole && !(value.toDouble() > 0))
    {
      emit errorMessageChanged(tr("Width and heights must be strictly positive"));
      return false;
    }

    return SampleTableModel::setData(index, value, role);
  }
};

class PERSALYS_UTILS_API ImportedDistributionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit ImportedDistributionPage(QWidget *parent = nullptr);
  bool validatePage() override;
  OT::Sample getData() const
  {
    return sampleWidget_->getData();
  };

private:
  ImportSampleWidget  * sampleWidget_ = nullptr;
  ErrorWidget         * errorWidget_  = nullptr;
};
}
#endif
