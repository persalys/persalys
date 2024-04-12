//                                               -*- C++ -*-
/**
 *  @brief QDialog to define values
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/TemporaryLabel.hxx"
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
  EditValuesWizard(QWidget *parent = nullptr);
  EditValuesWizard(const OT::Sample &values, QWidget *parent = nullptr);
  EditValuesWizard(const QString &variableName, const OT::Point &values, QWidget *parent = nullptr);

  OT::Point getValues(const OT::UnsignedInteger index = 0) const;
  virtual bool validateCurrentPage();

private slots:
  void removeSelectedValues();
  void checkButtons();
  virtual void addValue();
  virtual void importSample();

protected:
  void buildInterface();
  void check();

  SampleTableModel * model_;
  QSortFilterProxyModel * proxy_;
  TemporaryLabel * errorMessageLabel_;

private:
  OT::Description sampleDescription_;
  QTableView * valueTable_;
  QLabel * valueNumber_;
  QPushButton * removeButton_;
};


class PERSALYS_UTILS_API UserDefinedWizard : public EditValuesWizard
{
  Q_OBJECT

public:
  UserDefinedWizard(const OT::Distribution::PointWithDescriptionCollection &parameters, QWidget *parent = nullptr);

  OT::Distribution getDistribution() const;
  virtual bool validateCurrentPage();

private slots:
  virtual void addValue();
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

  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole)
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

class PERSALYS_UTILS_API ImportedDistributionPage : public QWizardPage
{
  Q_OBJECT

public:
  ImportedDistributionPage(QWidget *parent = nullptr);
  bool validatePage();
  OT::Sample getData() const
  {
    return sampleWidget_->getData();
  };

private:
  ImportSampleWidget * sampleWidget_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
