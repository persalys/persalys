// ExperimentTableModel.hxx

#ifndef EXPERIMENTTABLEMODEL_H
#define EXPERIMENTTABLEMODEL_H

#include "otgui/DesignOfExperiment.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class ExperimentTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  ExperimentTableModel(const DesignOfExperiment & designOfExperiment);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  DesignOfExperiment getDesignOfExperiment() const;

public slots:
signals:
  void designOfExperimentChanged(DesignOfExperiment&);

private:
  DesignOfExperiment designOfExperiment_;
};
}
#endif