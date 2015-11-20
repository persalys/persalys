// InputTableProbabilisticModel.hxx

#ifndef INPUTTABLEPROBABILISTICMODEL_H
#define INPUTTABLEPROBABILISTICMODEL_H

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>
#include <QVector>

namespace OTGUI {
class InputTableProbabilisticModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InputTableProbabilisticModel(const PhysicalModel & physicalModel, QObject * parent = 0);

  virtual ~InputTableProbabilisticModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  void addLine(int i);
  void removeLine(const QModelIndex & lineIndex);

private:
  PhysicalModel physicalModel_;
  QVector<int > userRoleList_;
};
}
#endif