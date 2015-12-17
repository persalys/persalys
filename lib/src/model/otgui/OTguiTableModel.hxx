// OTguiTableModel.hxx

#ifndef OTGUITABLEMODEL_H
#define OTGUITABLEMODEL_H

#include "NumericalSample.hxx"

#include <QAbstractTableModel>
#include <QModelIndex>


namespace OTGUI {
class OTguiTableModel : public QAbstractTableModel
{
public:
  OTguiTableModel(const OT::NumericalSample & data);

  virtual ~OTguiTableModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
  QVariant data(const QModelIndex & index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);

  bool exportData(const QString & fileName);

private:
  OT::NumericalSample data_;
};
}
#endif