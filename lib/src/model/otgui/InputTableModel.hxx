// InputTableModel.hxx

#ifndef PHYSICALMODELTABLEMODEL_H
#define PHYSICALMODELTABLEMODEL_H

#include "otgui/Input.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class InputTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InputTableModel(QObject * parent = 0);
  InputTableModel(const InputCollection & inputs);
  InputTableModel(const InputTableModel & other);

  virtual ~InputTableModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  void removeLine(const QModelIndex & index);
  InputCollection getData();
  bool isValid();

public slots:
  void addLine();
  void addLine(const Input & input);

private:
  InputCollection data_;
  bool validity_;
};
}
#endif