// InputTableProbabilisticModel.hxx

#ifndef INPUTTABLEPROBABILISTICMODEL_H
#define INPUTTABLEPROBABILISTICMODEL_H

#include "otgui/Input.hxx"

#include "DistributionFactory.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class InputTableProbabilisticModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InputTableProbabilisticModel(const InputCollection & inputs, QObject * parent = 0);
  InputTableProbabilisticModel(const InputTableProbabilisticModel & other);

  virtual ~InputTableProbabilisticModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);

  void addLine(const Input & input);
  InputCollection getData();
  OT::NumericalPointWithDescription getParameters(int row) const;
  void updateDistributionParameters(const QModelIndex & index, const OT::NumericalPoint & parameters);

private:
  static std::map<QString, OT::DistributionFactory> GetDistributionsMap();
  InputCollection data_;
};
}
Q_DECLARE_METATYPE(OT::DistributionFactory)
Q_DECLARE_METATYPE(OT::Distribution)
#endif