// InputTableModel.hxx

#ifndef PHYSICALMODELTABLEMODEL_H
#define PHYSICALMODELTABLEMODEL_H

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class InputTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InputTableModel(const PhysicalModel & physicalModel);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  void removeLine(const QModelIndex & index);
  bool isValid();

public slots:
  void addLine();

private:
  PhysicalModel physicalModel_;
};
}
#endif