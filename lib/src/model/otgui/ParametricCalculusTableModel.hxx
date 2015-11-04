// ParametricCalculusTableModel.hxx

#ifndef PARAMETRICCALCULUSTABLEMODEL_H
#define PARAMETRICCALCULUSTABLEMODEL_H

#include "otgui/ParametricCalculus.hxx"

#include <QAbstractTableModel>

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

public slots:
signals:
  void dataChanged(Calculus&);

private:
  Calculus calculus_;
};
}
#endif