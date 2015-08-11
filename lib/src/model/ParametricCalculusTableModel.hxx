#ifndef PARAMETRICCALCULUSTABLEMODEL_H
#define PARAMETRICCALCULUSTABLEMODEL_H

#include <QAbstractTableModel>
#include "ParametricCalculus.hxx"
#include <vector>

namespace OTGUI {
class ParametricCalculusTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  ParametricCalculusTableModel(const Calculus & calculus);

  virtual ~ParametricCalculusTableModel();

  Calculus getCalculus() const;
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void fillTable();

private:
  Calculus calculus_;
};
}
#endif