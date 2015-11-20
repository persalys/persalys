// OutputTableModel.hxx

#ifndef OUTPUTTABLEMODEL_H
#define OUTPUTTABLEMODEL_H

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>
#include <QModelIndex>

namespace OTGUI {
class OutputTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  OutputTableModel(const PhysicalModel & physicalModel);

  virtual ~OutputTableModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
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