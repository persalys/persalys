// CorrelationTableModel.hxx

#ifndef CORRELATIONTABLEMODEL_H
#define CORRELATIONTABLEMODEL_H

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class CorrelationTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  CorrelationTableModel(const PhysicalModel & physicalModel, QObject * parent = 0);

  virtual ~CorrelationTableModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  PhysicalModel getPhysicalModel() const;

private:
  PhysicalModel physicalModel_;
};
}
#endif